
#include "RHICommandQueue.h"

#include "RenderHardwareInterface.h"

namespace kgx::RHI
{
RHICommandQueue::RHICommandQueue()
{
	mFence = PlatformRHI->createFence();
}

void RHICommandQueue::flushQueue()
{
	mFence->sync();
}
}
