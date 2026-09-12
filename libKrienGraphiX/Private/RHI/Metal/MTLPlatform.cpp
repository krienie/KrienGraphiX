
#include "MTLPlatform.h"

#include "MTLRenderContext.h"

namespace kgx::RHI
{
MTLPlatform::MTLPlatform()
	: mAutoReleasePool(nullptr), mCommandQueue(*this)
{
	mCommandAllocatorPool = std::make_unique<CommandAllocatorPool>(5,
	[](core::ResourcePool<MTLCommandAllocator>& parentPool)
	{
		return std::make_unique<MTLCommandAllocator>(parentPool);
	});

	mCommandQueue.create();
}

MTLPlatform::~MTLPlatform()
{
	mCommandQueue.waitForCompletion();

	if (mAutoReleasePool)
	{
		mAutoReleasePool->release();
	}
}

void MTLPlatform::beginFrame(RHIRenderContext* renderContext, const RHITextureHandle& renderTarget)
{
	if (mAutoReleasePool)
	{
		mAutoReleasePool->release();
		mAutoReleasePool = nullptr;
	}

	mAutoReleasePool = NS::AutoreleasePool::alloc()->init();
}

void MTLPlatform::endFrame(RHIRenderContext* renderContext, const RHITextureHandle& renderTarget)
{
}

MTLCommandQueue& MTLPlatform::getCommandQueue()
{
	return mCommandQueue;
}

MTLCommandAllocator* MTLPlatform::getCommandAllocator() const
{
	return mCommandAllocatorPool->getResource();
}

std::unique_ptr<RHIRenderContext> MTLPlatform::createRenderContext(core::ResourcePool<RHIRenderContext>& parentPool) const
{
	auto renderContext = std::make_unique<MTLRenderContext>(parentPool, *const_cast<MTLPlatform*>(this));
	if (!renderContext->create())
	{
		return nullptr;
	}

	return renderContext;
}
}
