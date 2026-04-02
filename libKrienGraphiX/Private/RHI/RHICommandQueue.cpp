
#include "RHICommandQueue.h"

#include "RenderHardwareInterface.h"

namespace kgx::RHI
{
RHICommandQueue::RHICommandQueue()
{
	mFence = gPlatformRHI->createFence();
}

void RHICommandQueue::flushQueue()
{
	mFence->sync();
}
}
