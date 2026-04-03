
#include "MTLSwapChain.h"

#include <SDL3/SDL.h>
#include <Metal/MTL4ComputeCommandEncoder.hpp>

#include "MTLDescriptors.h"
#include "MTLFence.h"
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
	assert(bufferCount >= 1);
	
	auto autoReleasePool = NS::AutoreleasePool::alloc()->init();
	
	mCommandQueue = rcCast(commandQueue);

	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice()->getNativeDevice();
	mCommandBuffer = NS::TransferPtr(mtlDevice->newCommandBuffer());

	mMetalView = SDL_Metal_CreateView(window);
	mDrawLayer = static_cast<CA::MetalLayer*>(SDL_Metal_GetLayer(mMetalView));
	mDrawLayer->setDevice(mtlDevice);
	mDrawLayer->setPixelFormat(toMTLPixelFormat(pixelFormat));
	mDrawLayer->setDrawableSize(CGSizeMake(mWidth, mHeight));
	mDrawLayer->setMaximumDrawableCount(bufferCount);
	mDrawLayer->setFramebufferOnly(false);

	constexpr auto flags = static_cast<RHIResource::CreationFlags>(RHIResource::RenderTargetable | RHIResource::ShaderResource);

	const MTLTexture2DDescriptor textureDesc =
	{
		{
			{0, 0, 0, 0},
			pixelFormat,
			mWidth,
			mHeight,
			1,
			1,
			flags
		}
	};
	
	mNextPresentTextureIndex = bufferCount - 1;

	mOffscreenTextures.reserve(bufferCount);
	for (unsigned int i = 0; i < bufferCount; ++i)
	{
		const std::string textureLabel = "OffscreenTexture_" + std::to_string(i);
		
		auto newTexture = std::make_shared<MTLTexture2D>(textureDesc);
		newTexture->getNativeResource()->setLabel(NS::String::string(textureLabel.c_str(), NS::UTF8StringEncoding));
		mOffscreenTextures.push_back(newTexture);
		mTextureViews.push_back(std::make_shared<MTLTextureView>(RHIResourceView::Type::RTV, newTexture));
		mCommandAllocators.push_back(NS::TransferPtr(mtlDevice->newCommandAllocator()));

		mCommandQueue->getResidencySet()->addAllocation(newTexture->getNativeResource());
	}
	
	mCommandQueue->getNativeCommandQueue()->addResidencySet(mDrawLayer->residencySet());
	mCommandQueue->getResidencySet()->commit();
	autoReleasePool->release();

	clearOffscreenTextures();

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
	MTL4::CommandAllocator* currentAllocator = mCommandAllocators[mNextPresentTextureIndex].get();
	currentAllocator->reset();

	mCommandQueue->getNativeCommandQueue()->wait(drawable);

	mCommandBuffer->beginCommandBuffer(currentAllocator);

	MTL4::ComputeCommandEncoder* computeEncoder = mCommandBuffer->computeCommandEncoder();
	computeEncoder->copyFromTexture(mOffscreenTextures[mNextPresentTextureIndex]->getNativeResource(),
		drawable->texture());
	computeEncoder->endEncoding();

	mCommandBuffer->endCommandBuffer();

	MTL4::CommandBuffer* cmdBuffPtr = mCommandBuffer.get();
	mCommandQueue->getNativeCommandQueue()->commit(&cmdBuffPtr, 1);
	mCommandQueue->getNativeCommandQueue()->signalDrawable(drawable);
	drawable->present();

	mCurrentTextureIndex = (mCurrentTextureIndex + 1) % mTextureViews.size();
	mNextPresentTextureIndex = (mNextPresentTextureIndex + 1) % mTextureViews.size();
}

void MTLSwapChain::clearOffscreenTextures() const
{
	auto autoReleasePool = NS::AutoreleasePool::alloc()->init();

	MTL::RenderPassDescriptor* pRpd = MTL::RenderPassDescriptor::renderPassDescriptor();

	for (int i = 0; i < mTextureViews.size(); ++i)
	{
		auto colorAttach = pRpd->colorAttachments()->object(i);

		MTLTexture2D* offscreenTexture = rcCast(mTextureViews[i]->getViewedResource());
		colorAttach->setTexture(offscreenTexture->getNativeResource());
		colorAttach->setLoadAction(MTL::LoadActionClear);
		colorAttach->setClearColor(MTL::ClearColor::Make(0, 0, 0, 1)); // Black with alpha 1
		colorAttach->setStoreAction(MTL::StoreActionStore);
	}

	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice()->getNativeDevice();

	auto commandAllocator = NS::TransferPtr(mtlDevice->newCommandAllocator());
	auto commandBuffer = NS::TransferPtr(mtlDevice->newCommandBuffer());
	commandBuffer->beginCommandBuffer(commandAllocator.get());
	auto commandEncoder = commandBuffer->renderCommandEncoder(reinterpret_cast<MTL4::RenderPassDescriptor*>(pRpd));
	commandEncoder->endEncoding();
	commandBuffer->endCommandBuffer();

	MTL4::CommandBuffer* cmdBuffPtr = commandBuffer.get();
	mCommandQueue->getNativeCommandQueue()->commit(&cmdBuffPtr, 1);
	mCommandQueue->waitForCompletion();

	autoReleasePool->release();
}
}
