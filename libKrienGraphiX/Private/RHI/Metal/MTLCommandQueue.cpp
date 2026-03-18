
#include "MTLCommandQueue.h"

#include "MTLGraphicsCommandList.h"
#include "MTLRenderHardwareInterface.h"

namespace kgx::RHI
{
MTLCommandQueue::MTLCommandQueue()
	: RHICommandQueue(), mCommandQueue(nullptr)
{
}

bool MTLCommandQueue::create()
{
	MTL::Device* nativeDevice = getMTLRHI()->getMTLDevice();
	mCommandQueue = NS::TransferPtr(nativeDevice->newMTL4CommandQueue());

	return mCommandQueue.get() != nullptr;
}

void MTLCommandQueue::executeCommandList(RHIGraphicsCommandList* commandList)
{
	MTLGraphicsCommandList* mtlCommandList = rcCast(commandList);

	MTL4::CommandBuffer* ppCommandLists[] = { mtlCommandList->getCommandBuffer() };
	mCommandQueue->commit(ppCommandLists, 1);
}
}
