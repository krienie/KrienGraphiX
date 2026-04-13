
#include "DX12Fence.h"

#include "DX12CommandQueue.h"
#include "DX12GraphicsDevice.h"
#include "DX12RenderHardwareInterface.h"

#include "Private/Core/RenderCore.h"

namespace kgx::RHI
{
DX12Fence::DX12Fence()
{
	ID3D12Device* nativeDevice = getDX12RHI()->getDX12Device()->getNativeDevice();
	nativeDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));

	mEventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

	//TODO(KL): Log error if failed
}

DX12Fence::~DX12Fence()
{
	CloseHandle(mEventHandle);
}

void DX12Fence::sync()
{
	++mFenceValue;
	queueSignal(mFenceValue);
	waitForValue(mFenceValue);
}

void DX12Fence::queueSignal(uint64_t value)
{
	DX12CommandQueue* commandQueue = rcCast(core::gRenderThread->getCommandQueuePtr());
	commandQueue->getNativeCommandQueue()->Signal(mFence.Get(), value);
}

void DX12Fence::waitForValue(uint64_t value)
{
	if (mFence->GetCompletedValue() < value)
	{
		mFence->SetEventOnCompletion(value, mEventHandle);

		WaitForSingleObject(mEventHandle, INFINITE);
	}
}

uint64_t DX12Fence::getCurrentValue() const
{
	return mFence->GetCompletedValue();
}
}
