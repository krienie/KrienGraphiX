
#include "MTLCommandQueue.h"

#include "MTLRenderHardwareInterface.h"
#include "MTLFence.h"

namespace kgx::RHI
{
MTLCommandQueue::MTLCommandQueue(MTLPlatform& platform)
	: mCommandQueue(nullptr), mPlatform(platform)
{
}

void MTLCommandQueue::addGlobalResidency(const MTL::Allocation* allocation)
{
	mResidencySetDirty = true;
	mResidencySet->addAllocation(allocation);
}

bool MTLCommandQueue::create()
{
	auto autoReleasePool = NS::AutoreleasePool::alloc()->init();

	mFence = std::make_unique<MTLFence>(mPlatform);

	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice()->getNativeDevice();
	mCommandQueue = NS::TransferPtr(mtlDevice->newMTL4CommandQueue());

	NS::SharedPtr<MTL::ResidencySetDescriptor> resDesc = NS::TransferPtr(MTL::ResidencySetDescriptor::alloc()->init());
	resDesc->setLabel(NS::String::string("GlobalQueueResidencySet", NS::UTF8StringEncoding));

	NS::Error* error = nullptr;
	mResidencySet = NS::TransferPtr(mtlDevice->newResidencySet(resDesc.get(), &error));

	mCommandQueue->addResidencySet(mResidencySet.get());

	autoReleasePool->release();

	return mCommandQueue.get() != nullptr;
}

void MTLCommandQueue::executeCommandBuffer(MTL4::CommandBuffer* commandBuffer, bool waitForCompletion)
{
	if (mResidencySetDirty)
	{
		mResidencySet->commit();
		mResidencySetDirty = false;
	}

	MTL4::CommandBuffer* ppCommandLists[] = { commandBuffer };
	mCommandQueue->commit(ppCommandLists, 1);

	if (waitForCompletion)
	{
		mFence->sync();
	}
}

void MTLCommandQueue::waitForCompletion() const
{
	mFence->sync();
}
}
