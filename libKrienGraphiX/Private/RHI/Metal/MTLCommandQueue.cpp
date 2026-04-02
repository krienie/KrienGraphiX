
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
	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice()->getNativeDevice();
	mCommandQueue = NS::TransferPtr(mtlDevice->newMTL4CommandQueue());

	NS::SharedPtr<MTL::ResidencySetDescriptor> resDesc = NS::TransferPtr(MTL::ResidencySetDescriptor::alloc()->init());
	resDesc->setLabel(NS::String::string("GlobalQueueResidencySet", NS::UTF8StringEncoding));

	NS::Error* pError = nullptr;
	mResidencySet = NS::TransferPtr(mtlDevice->newResidencySet(resDesc.get(), &pError));

	mCommandQueue->addResidencySet(mResidencySet.get());

	return mCommandQueue.get() != nullptr;
}

void MTLCommandQueue::executeCommandList(RHIGraphicsCommandList* commandList)
{
	MTLGraphicsCommandList* mtlCommandList = rcCast(commandList);

	MTL4::CommandBuffer* ppCommandLists[] = { mtlCommandList->getCommandBuffer() };
	mCommandQueue->commit(ppCommandLists, 1);
}
}
