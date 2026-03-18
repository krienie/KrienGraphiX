
#include "MTLFence.h"

#include "MTLCommandQueue.h"
#include "MTLRenderHardwareInterface.h"
#include "Private/Core/RenderCore.h"

namespace kgx::RHI
{
MTLFence::MTLFence()
{
	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice();
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
	MTLCommandQueue* commandQueue = rcCast(core::RenderCore::get()->getRenderThreadPtr()->getCommandQueuePtr());
	commandQueue->getNativeCommandQueue()->signalEvent(mEvent.get(), mFenceValue);
}

void MTLFence::waitForValue(uint64_t value)
{
	mEvent->waitUntilSignaledValue(mFenceValue, UINT64_MAX);
}

uint64_t MTLFence::getCurrentValue() const
{
	return mEvent->signaledValue();
}
}
