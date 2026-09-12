
#include "RHIPlatform.h"

namespace kgx::RHI
{
void RHIPlatform::init()
{
	mRenderContextPool = std::make_unique<RenderContextPool>(5, [this](core::ResourcePool<RHIRenderContext>& parentPool)
	{
		return createRenderContext(parentPool);
	});
}

RHITextureHandle RHIPlatform::registerTexture(std::unique_ptr<RHITexture2D>&& texture)
{
	return mTextureRegistry.registerTexture(std::forward<std::unique_ptr<RHITexture2D>>(texture));
}

RHITexture2D* RHIPlatform::getTexture2D(const RHITextureHandle& handle) const
{
	return mTextureRegistry.get(handle);
}

RHIRenderContext* RHIPlatform::getRenderContext() const
{
	return mRenderContextPool->getResource();
}
}
