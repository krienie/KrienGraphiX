
#include "MTLFence.h"

#include "MTLCommandQueue.h"
#include "MTLPlatform.h"
#include "MTLRenderHardwareInterface.h"

namespace kgx::RHI
{
MTLFence::MTLFence(MTLPlatform& platform)
{
	mCommandQueue = &platform.getCommandQueue();

	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice()->getNativeDevice();
	mEvent = NS::TransferPtr(mtlDevice->newSharedEvent());
}

void MTLFence::sync()
{
	++mFenceValue;
	queueSignal(mFenceValue);
	waitForValue(mFenceValue);
}

void MTLFence::queueSignal(uint64_t value)
{
	mCommandQueue->getNativeCommandQueue()->signalEvent(mEvent.get(), value);
}

void MTLFence::waitForValue(uint64_t value)
{
	mEvent->waitUntilSignaledValue(value, UINT64_MAX);
}

uint64_t MTLFence::getCurrentValue() const
{
	return mEvent->signaledValue();
}
}
