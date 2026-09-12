
#pragma once

#include <memory>

#include "RHIRenderContext.h"
#include "RHITextureRegistry.h"
#include "RHITextureHandle.h"
#include "RHITexture2D.h"
#include "Private/Core/ResourcePool.h"

namespace kgx::RHI
{
class RHIPlatform
{
public:
	using RenderContextPool = core::ResourcePool<RHIRenderContext>;

	RHIPlatform() = default;
	virtual ~RHIPlatform() = default;

	RHIPlatform(const RHIPlatform&) noexcept            = delete;
	RHIPlatform(RHIPlatform&&) noexcept                 = delete;
	RHIPlatform& operator=(const RHIPlatform&) noexcept = delete;
	RHIPlatform& operator=(RHIPlatform&&) noexcept      = delete;

	void init();

	[[nodiscard]] RHITextureHandle registerTexture(std::unique_ptr<RHITexture2D>&& texture);
	[[nodiscard]] RHITexture2D* getTexture2D(const RHITextureHandle& handle) const;

	[[nodiscard]]
	RHIRenderContext* getRenderContext() const;

	virtual void beginFrame(RHIRenderContext* renderContext, const RHITextureHandle& renderTarget) = 0;
	virtual void endFrame(RHIRenderContext* renderContext, const RHITextureHandle& renderTarget) = 0;

protected:
	std::unique_ptr<RenderContextPool> mRenderContextPool;

private:
	[[nodiscard]]
	virtual std::unique_ptr<RHIRenderContext> createRenderContext(core::ResourcePool<RHIRenderContext>& parentPool) const = 0;

	RHITextureRegistry mTextureRegistry;
};
}
