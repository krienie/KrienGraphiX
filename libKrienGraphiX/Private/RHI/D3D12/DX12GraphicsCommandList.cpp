
#include "DX12GraphicsCommandList.h"

#include "DX12DepthStencilBuffer.h"
#include "DX12GraphicsDevice.h"
#include "DX12GraphicsPipelineState.h"

namespace kgx::RHI
{
DX12GraphicsCommandList::DX12GraphicsCommandList()
    : RHIGraphicsCommandList(), mCommandList(nullptr), mCommandAllocator(nullptr)
{
}

bool DX12GraphicsCommandList::init(RHIGraphicsDevice *device, RHIGraphicsPipelineState * initialState)
{
    auto * dxDevice = dynamic_cast<DX12GraphicsDevice*>(device);
    if (dxDevice == nullptr)
    {
        // This should never happen
        return false;
    }

    auto * nativeDevice = dxDevice->getNativeDevice();

    // For now, each CommandList has its own CommandAllocator
    HRESULT res = nativeDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
    if (FAILED(res))
    {
        return false;
    }

    ID3D12PipelineState * nativeInitialState = nullptr;

    auto * dxPipelineState = dynamic_cast<DX12GraphicsPipelineState*>(initialState);
    if (dxPipelineState != nullptr)
    {
        nativeInitialState = dxPipelineState->getPipelineState();
    }

    res = nativeDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nativeInitialState, IID_PPV_ARGS(&mCommandList));
    return SUCCEEDED(res);
}

void DX12GraphicsCommandList::close()
{
    mCommandList->Close();
}

ID3D12CommandList * DX12GraphicsCommandList::getCommandList() const
{
    return mCommandList.Get();
}

ID3D12CommandAllocator * DX12GraphicsCommandList::getCommandAllocator() const
{
    return mCommandAllocator.Get();
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