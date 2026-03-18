
#pragma once

#include <memory>

#include <SDL3/SDL.h>

#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTL4CommandAllocator.hpp>
#include <Metal/MTL4CommandBuffer.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include "MTLCommandQueue.h"
#include "MTLTexture2D.h"
#include "MTLTextureView.h"
#include "Private/RHI/RHISwapChain.h"
#include "Private/RHI/RHIUtils.h"

namespace kgx::RHI
{
class MTLSwapChain : public RHISwapChain
{
public:
	MTLSwapChain(uint32_t width, uint32_t height);
	~MTLSwapChain() override;

	bool create(RHICommandQueue* commandQueue, SDL_Window* window, unsigned int bufferCount, RHIPixelFormat pixelFormat) override;

	[[nodiscard]] RHIResourceView* getCurrentBufferView() override;

	void present() override;

private:
	uint32_t mWidth;
	uint32_t mHeight;
	CA::MetalLayer* mDrawLayer = nullptr;
	MTLCommandQueue* mCommandQueue = nullptr;
	SDL_MetalView mMetalView = nullptr;

	uint8_t mCurrentTextureIndex = 0;
	std::vector<std::shared_ptr<MTLTexture2D>> mOffscreenTextures;
	std::vector<std::shared_ptr<MTLTextureView>> mTextureViews;

	NS::SharedPtr<MTL4::CommandBuffer> mCommandBuffer = nullptr;
	std::vector<NS::SharedPtr<MTL4::CommandAllocator>> mCommandAllocators;
};

DEFINE_RESOURCE_CAST(MTLSwapChain, RHISwapChain);
}
