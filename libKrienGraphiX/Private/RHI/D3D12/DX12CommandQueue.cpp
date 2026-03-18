
#include "DX12CommandQueue.h"

#include "DX12GraphicsDevice.h"
#include "DX12RenderHardwareInterface.h"
#include "Private/RHI/D3D12/DX12GraphicsCommandList.h"

namespace kgx::RHI
{
DX12CommandQueue::DX12CommandQueue()
	: RHICommandQueue(), mCommandQueue(nullptr)
{
}

bool DX12CommandQueue::create()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ID3D12Device* nativeDevice = getDX12RHI()->getDX12Device()->getNativeDevice();

	HRESULT res = nativeDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
	if (FAILED(res))
	{
		return false;
	}

	return SUCCEEDED(res);
}

void DX12CommandQueue::executeCommandList(RHIGraphicsCommandList* commandList)
{
	DX12GraphicsCommandList* dxCommandList = rcCast(commandList);

	ID3D12CommandList* ppCommandLists[] = { dxCommandList->getCommandList() };
	mCommandQueue->ExecuteCommandLists(1u, ppCommandLists);
}

ID3D12CommandQueue* DX12CommandQueue::getNativeCommandQueue() const
{
	return mCommandQueue.Get();
}
}