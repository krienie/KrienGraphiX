
#include "DX12GraphicsCommandList.h"

#include <cassert>

#include "DX12CommandQueue.h"
#include "DX12DepthStencilBuffer.h"
#include "DX12GraphicsDevice.h"
#include "DX12GraphicsPipelineState.h"

namespace kgx::RHI
{
DX12GraphicsCommandList::DX12GraphicsCommandList()
    : RHIGraphicsCommandList(), mCommandList(nullptr)
{
}

bool DX12GraphicsCommandList::init(RHIGraphicsDevice *device, RHICommandQueue* commandQueue, RHIGraphicsPipelineState * initialState)
{
    auto * dxDevice = dynamic_cast<DX12GraphicsDevice*>(device);
    assert(dxDevice);

    auto * nativeDevice = dxDevice->getNativeDevice();

    ID3D12PipelineState * nativeInitialState = nullptr;

    auto * dxPipelineState = dynamic_cast<DX12GraphicsPipelineState*>(initialState);
    if (dxPipelineState != nullptr)
    {
        nativeInitialState = dxPipelineState->getPipelineState();
    }

    auto dxCommandQueue = dynamic_cast<DX12CommandQueue*>(commandQueue);
    assert(dxCommandQueue);

    HRESULT res = nativeDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, dxCommandQueue->getNativeCommandAllocator(), nativeInitialState, IID_PPV_ARGS(&mCommandList));
    return SUCCEEDED(res);
}

void DX12GraphicsCommandList::close()
{
    mCommandList->Close();
}

void DX12GraphicsCommandList::reset(RHICommandQueue* commandQueue)
{
    auto dxCommandQueue = dynamic_cast<DX12CommandQueue*>(commandQueue);
    assert(dxCommandQueue);

    mCommandList->Reset(dxCommandQueue->getNativeCommandAllocator(), nullptr);
}

void DX12GraphicsCommandList::setViewport(const KGXViewport& viewport)
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

    const D3D12_RECT scissorRect = { viewport.topLeftX, viewport.topLeftY, viewport.width, viewport.height };
    mCommandList->RSSetScissorRects(1u, &scissorRect);
}

ID3D12CommandList * DX12GraphicsCommandList::getCommandList() const
{
    return mCommandList.Get();
}

void DX12GraphicsCommandList::clearDepthStencilView(RHIDepthStencilBuffer *dsv /*, clearFlags, depth, stencil*/)
{
    auto * dxDsv = dynamic_cast<DX12DepthStencilBuffer*>(dsv);
    if (dxDsv != nullptr)
    {
        const auto heapStart = dxDsv->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
        mCommandList->ClearDepthStencilView(heapStart, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }
}

void DX12GraphicsCommandList::clearRenderTargetView()
{
    //mCommandList->ClearRenderTargetView();
}

} // namespace kgx::RHI