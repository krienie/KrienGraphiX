
#include "DX12CommandQueue.h"

#include "DX12GraphicsDevice.h"
#include "Private/RHI/RHIGraphicsCommandList.h"
#include "Private/RHI/D3D12/DX12GraphicsCommandList.h"

namespace kgx::RHI
{
DX12CommandQueue::DX12CommandQueue()
    : RHICommandQueue(), mCommandQueue(nullptr)
{
}

bool DX12CommandQueue::init(RHIGraphicsDevice *device)
{
    auto * dxDevice = dynamic_cast<DX12GraphicsDevice*>(device);
    if (dxDevice == nullptr)
    {
        // This should never happen
        return false;
    }

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    const HRESULT res = dxDevice->getNativeDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));

    return SUCCEEDED(res);
}

void DX12CommandQueue::executeCommandList(RHIGraphicsCommandList* commandList)
{
    auto * dxCommandList = dynamic_cast<DX12GraphicsCommandList*>(commandList);
    if (dxCommandList == nullptr)
    {
        // This should never happen
        return;
    }

    ID3D12CommandList* ppCommandLists[] = { dxCommandList->getCommandList() };
    mCommandQueue->ExecuteCommandLists(1u, ppCommandLists);
}

ID3D12CommandQueue * DX12CommandQueue::getNativeCommandQueue() const
{
    return mCommandQueue.Get();
}

} // namespace kgx::RHI