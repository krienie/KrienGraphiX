
#include "DX12CommandQueue.h"

#include <cassert>

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

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    auto* nativeDevice = static_cast<DX12GraphicsDevice*>(device)->getNativeDevice();

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

    nativeDevice->CreateFence(mCurrentFence, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));

    return SUCCEEDED(res);
}

void DX12CommandQueue::executeCommandList(RHIGraphicsCommandList* commandList)
{
    auto* dxCommandList = static_cast<DX12GraphicsCommandList*>(commandList);

    ID3D12CommandList* ppCommandLists[] = { dxCommandList->getCommandList() };
    mCommandQueue->ExecuteCommandLists(1u, ppCommandLists);
}

void DX12CommandQueue::flushQueue()
{
    ++mCurrentFence;
    
    mCommandQueue->Signal(mFence.Get(), mCurrentFence);

	// Wait for fence event if needed
    if (mFence->GetCompletedValue() < mCurrentFence)
	{
        const HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

        mFence->SetEventOnCompletion(mCurrentFence, eventHandle);

		WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
	}
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