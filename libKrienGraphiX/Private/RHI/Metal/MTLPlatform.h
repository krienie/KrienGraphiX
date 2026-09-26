
#pragma once

#include "MTLCommandAllocator.h"
#include "MTLCommandQueue.h"
#include "Private/RHI/RHIPlatform.h"

namespace kgx::RHI
{
class MTLPlatform : public RHIPlatform
{
public:
	MTLPlatform(const class MTLGraphicsDevice* mtlDevice);
	~MTLPlatform() override;

	void beginFrame(RHIRenderContext* renderContext, const RHITextureHandle& renderTarget) override;
	void endFrame(RHIRenderContext* renderContext, const RHITextureHandle& renderTarget) override;

	[[nodiscard]] MTLCommandQueue& getCommandQueue();
	[[nodiscard]] MTLCommandAllocator* getCommandAllocator() const;

	[[nodiscard]] unsigned int getMultisampleCount() const;

private:
	using CommandAllocatorPool = core::ResourcePool<MTLCommandAllocator>;

	[[nodiscard]]
	std::unique_ptr<RHIRenderContext> createRenderContext(core::ResourcePool<RHIRenderContext>& parentPool) const override;

	NS::AutoreleasePool* mAutoReleasePool;
	MTLCommandQueue mCommandQueue;
	std::unique_ptr<CommandAllocatorPool> mCommandAllocatorPool;

	unsigned int mMultisampleCount;
};
}
