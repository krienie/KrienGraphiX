
#include "DX12CommandQueue.h"

#include "DX12GraphicsDevice.h"
#include "Private/RHI/RHIGraphicsCommandList.h"
#include "Private/RHI/D3D12/DX12GraphicsCommandList.h"

namespace kgx::RHI
{
DX12CommandQueue::DX12CommandQueue()
    : RHICommandQueue(), mCommandQueue(nullptr), mCommandAllocator(nullptr)
{
}

bool DX12CommandQueue::init(RHIGraphicsDevice* device)
{
    //TODO(KL): Think of a way to remove all these dynamic_casts...
    auto* dxDevice = dynamic_cast<DX12GraphicsDevice*>(device);
    if (dxDevice == nullptr)
    {
        // This should never happen
        return false;
    }

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    auto* nativeDevice = dxDevice->getNativeDevice();

    HRESULT res = nativeDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
    if (FAILED(res))
    {
        return false;
    }

    res = nativeDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
    if (FAILED(res))
    {
        return false;
    }

    return SUCCEEDED(res);
}

void DX12CommandQueue::executeCommandList(RHIGraphicsCommandList* commandList)
{
    auto* dxCommandList = dynamic_cast<DX12GraphicsCommandList*>(commandList);
    if (dxCommandList == nullptr)
    {
        // This should never happen
        return;
    }

    ID3D12CommandList* ppCommandLists[] = { dxCommandList->getCommandList() };
    mCommandQueue->ExecuteCommandLists(1u, ppCommandLists);
}

ID3D12CommandQueue* DX12CommandQueue::getNativeCommandQueue() const
{
    return mCommandQueue.Get();
}

ID3D12CommandAllocator* DX12CommandQueue::getNativeCommandAllocator() const
{
    return mCommandAllocator.Get();
}
} // namespace kgx::RHI