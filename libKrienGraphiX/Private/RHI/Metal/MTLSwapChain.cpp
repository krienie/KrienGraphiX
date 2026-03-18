
#include "MTLSwapChain.h"

#include <SDL3/SDL.h>
#include <Metal/MTL4ComputeCommandEncoder.hpp>

#include "MTLDescriptors.h"
#include "MTLPixelFormat.h"
#include "MTLRenderHardwareInterface.h"

namespace kgx::RHI
{
MTLSwapChain::MTLSwapChain(uint32_t width, uint32_t height)
	: mWidth(width), mHeight(height)
{
}

MTLSwapChain::~MTLSwapChain()
{
	if (mMetalView)
	{
		SDL_Metal_DestroyView(mMetalView);
	}
}

bool MTLSwapChain::create(RHICommandQueue* commandQueue, SDL_Window* window, unsigned int bufferCount, RHIPixelFormat pixelFormat)
{
	mCommandQueue = rcCast(commandQueue);

	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice();
	mCommandBuffer = NS::TransferPtr(mtlDevice->newCommandBuffer());

	mMetalView = SDL_Metal_CreateView(window);
	mDrawLayer = static_cast<CA::MetalLayer*>(SDL_Metal_GetLayer(mMetalView));
	mDrawLayer->setDevice(getMTLRHI()->getMTLDevice());
	mDrawLayer->setPixelFormat(toMTLPixelFormat(pixelFormat));
	mDrawLayer->setDrawableSize(CGSizeMake(mWidth, mHeight));
	mDrawLayer->setMaximumDrawableCount(bufferCount);
    mDrawLayer->setFramebufferOnly(false);

	const MTLTexture2DDescriptor textureDesc =
	{
		{
			{0, 0, 0, 0},
			pixelFormat,
			mWidth,
			mHeight
		}
	};

	mOffscreenTextures.reserve(bufferCount);
	for (unsigned int i = 0; i < bufferCount; ++i)
	{
		auto newTexture = std::make_shared<MTLTexture2D>(textureDesc);
		mOffscreenTextures.push_back(newTexture);
		mTextureViews.push_back(std::make_shared<MTLTextureView>(RHIResourceView::Type::RTV, newTexture));

		mCommandAllocators.push_back(NS::TransferPtr(mtlDevice->newCommandAllocator()));
	}

	return true;
}

RHIResourceView* MTLSwapChain::getCurrentBufferView()
{
	return mTextureViews[mCurrentTextureIndex].get();
}

void MTLSwapChain::present()
{
	CA::MetalDrawable* drawable = mDrawLayer->nextDrawable();
	if (!drawable)
	{
		return;
	}

	// Copy from the offscreen texture to the current drawable so it can be shown on screen
	MTL4::CommandAllocator* currentAllocator = mCommandAllocators[mCurrentTextureIndex].get();
	currentAllocator->reset();

	mCommandQueue->getNativeCommandQueue()->wait(drawable);

	mCommandBuffer->beginCommandBuffer(currentAllocator);
    
	MTL4::ComputeCommandEncoder* computeEncoder = mCommandBuffer->computeCommandEncoder();
	computeEncoder->copyFromTexture(mOffscreenTextures[mCurrentTextureIndex]->getNativeResource(),
		drawable->texture());
	computeEncoder->endEncoding();
    
    mCommandBuffer->endCommandBuffer();

	MTL4::CommandBuffer* cmdBuffPtr = mCommandBuffer.get();
	mCommandQueue->getNativeCommandQueue()->commit(&cmdBuffPtr, 1);
	mCommandQueue->getNativeCommandQueue()->signalDrawable(drawable);
	drawable->present();

	mCurrentTextureIndex = (mCurrentTextureIndex + 1) % mTextureViews.size();
}
}
