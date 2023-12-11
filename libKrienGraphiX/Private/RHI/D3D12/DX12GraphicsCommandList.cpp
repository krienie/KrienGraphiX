
#include "DX12GraphicsCommandList.h"

#include <cassert>

#include "DX12CommandQueue.h"
#include "DX12GraphicsDevice.h"
#include "DX12GraphicsPipelineState.h"
#include "DX12ResourceView.h"

namespace
{
D3D12_CLEAR_FLAGS toDxClearFlags(kgx::RHI::DepthStencilFlags flags)
{
    using DepthStencilFlags = kgx::RHI::DepthStencilFlags;

    assert(flags & DepthStencilFlags::DepthClear | flags & DepthStencilFlags::StencilClear);

    D3D12_CLEAR_FLAGS dxClearFlags = flags & DepthStencilFlags::DepthClear ? D3D12_CLEAR_FLAG_DEPTH : static_cast<D3D12_CLEAR_FLAGS>(0);
    dxClearFlags |= flags & DepthStencilFlags::StencilClear ? D3D12_CLEAR_FLAG_STENCIL : static_cast<D3D12_CLEAR_FLAGS>(0);

    return dxClearFlags;
}
}

namespace kgx::RHI
{
DX12GraphicsCommandList::DX12GraphicsCommandList()
    : RHIGraphicsCommandList(), mCommandList(nullptr)
{
}

bool DX12GraphicsCommandList::create(RHIGraphicsDevice* device, RHICommandQueue* commandQueue, RHIGraphicsPipelineState* initialState)
{
    auto* nativeDevice = static_cast<DX12GraphicsDevice*>(device)->getNativeDevice();

    ID3D12PipelineState* nativeInitialState = nullptr;

    if (initialState != nullptr)
    {
        nativeInitialState = static_cast<DX12GraphicsPipelineState*>(initialState)->getPSO();
    }
    
    const auto dxCommandQueue = static_cast<DX12CommandQueue*>(commandQueue);

    const HRESULT res = nativeDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, dxCommandQueue->getNativeCommandAllocator(), nativeInitialState, IID_PPV_ARGS(&mCommandList));
    if (SUCCEEDED(res))
    {
        // Start off closed, because the first thing we do when rendering is to close the commandallocator that was used in the previous frame.
        mCommandList->Close();
    }

    return SUCCEEDED(res);
}

void DX12GraphicsCommandList::close()
{
    mCommandList->Close();
}

void DX12GraphicsCommandList::reset(RHICommandQueue* commandQueue, RHIGraphicsPipelineState* initialState)
{
    auto dxCommandQueue = static_cast<DX12CommandQueue*>(commandQueue);

    ID3D12PipelineState* nativeInitialState = nullptr;

    if (initialState != nullptr)
    {
        auto* dxPipelineState = static_cast<DX12GraphicsPipelineState*>(initialState);
        nativeInitialState = dxPipelineState->getPSO();
    }
    

    mCommandList->Reset(dxCommandQueue->getNativeCommandAllocator(), nativeInitialState);
}

void DX12GraphicsCommandList::setPipelineState(RHIGraphicsPipelineState* pipelineState)
{
    auto* dxPipelineState = static_cast<DX12GraphicsPipelineState*>(pipelineState);
    ID3D12PipelineState* nativePipelineState = dxPipelineState->getPSO();
    mCommandList->SetPipelineState(nativePipelineState);
}

void DX12GraphicsCommandList::setViewport(const core::KGXViewport& viewport)
{
    const D3D12_VIEWPORT dxViewport =
    {
        static_cast<float>(viewport.topLeftX),
        static_cast<float>(viewport.topLeftY),
        static_cast<float>(viewport.width),
        static_cast<float>(viewport.height),
        viewport.minDepth,
        viewport.maxDepth
    };
    mCommandList->RSSetViewports(1u, &dxViewport);

    const D3D12_RECT scissorRect = { viewport.topLeftX, viewport.topLeftY, static_cast<long>(viewport.width), static_cast<long>(viewport.height) };
    mCommandList->RSSetScissorRects(1u, &scissorRect);
}

void DX12GraphicsCommandList::setRenderTargets(const std::vector<RHIResourceView*>& renderTargetViews, const RHIResourceView* depthStencilView)
{
    auto* dxDsv = static_cast<const DX12ResourceView*>(depthStencilView);
    assert(dxDsv->getViewType() == RHIResourceView::DSV);

    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvCpuHandles(renderTargetViews.size());
    for (uint8_t i = 0u; i < static_cast<uint8_t>(renderTargetViews.size()); ++i)
    {
        auto dxRtv = static_cast<DX12ResourceView*>(renderTargetViews[i]);
        assert(dxRtv->getViewType() == RHIResourceView::RTV);

        rtvCpuHandles[i] = dxRtv->getViewHandle();
    }

    const D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuHandle = dxDsv->getViewHandle();

    mCommandList->OMSetRenderTargets(static_cast<UINT>(rtvCpuHandles.size()), rtvCpuHandles.data(), false, &dsvCpuHandle);
}

void DX12GraphicsCommandList::clearDepthStencilView(RHIResourceView* dsv, DepthStencilFlags clearFlags, float depth, uint8_t stencil)
{
    auto* dxDsv = static_cast<DX12ResourceView*>(dsv);
    assert(dxDsv->getViewType() == RHIResourceView::DSV);
    
    mCommandList->ClearDepthStencilView(dxDsv->getViewHandle(), toDxClearFlags(clearFlags), depth, stencil, 0, nullptr);
}

void DX12GraphicsCommandList::clearRenderTargetView(RHIResourceView* rtv, const float colorRGBA[4])
{
    auto* dxRtv = static_cast<DX12ResourceView*>(rtv);
    assert(dxRtv->getViewType() == RHIResourceView::RTV);

    mCommandList->ClearRenderTargetView(dxRtv->getViewHandle(), colorRGBA, 0, nullptr);
}

} // namespace kgx::RHI