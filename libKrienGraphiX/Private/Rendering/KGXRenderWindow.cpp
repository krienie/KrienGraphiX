
#include "KGXRenderWindow.h"

#include "KGXRenderer.h"
#include "Private/RHI/RenderHardwareInterface.h"
#include "Private/Core/RenderCore.h"
#include "Private/Core/RenderThread.h"

namespace kgx::rendering
{
KGXRenderWindow::KGXRenderWindow(SDL_Window* window, unsigned int width, unsigned int height)
	: mWindowHandle(window), mRHISwapChain(nullptr)
{
	using namespace RHI;
	mRHISwapChain = gPlatformRHI->createSwapChain(
		mWindowHandle,
		width,
		height,
		core::RenderThread::maxNumBufferedFrames);

	mViewport =
	{
		.topLeftX = 0,
		.topLeftY = 0,
		.width = width,
		.height = height,
		.minDepth = 1.0f,
		.maxDepth = 0.0f
	};

	const RHITexture2DDescriptor texDesc =
	{
		.clearValue = RHIClearValue{.depthClear = RHIClearValue::DepthClear{.depth = 0.0f, .stencil = 0}},
		.pixelFormat = RHIPixelFormat::R24_unorm_X8_typeless,
		.width = width,
		.height = height,
		.numMips = 1,
		.numSamples = 1,
		.flags = static_cast<RHIResource::CreationFlags>(RHIResource::DepthStencil | RHIResource::RenderTargetable)
	};

	mDepthStencil = gPlatformRHI->createTexture2D(texDesc);
}

void KGXRenderWindow::draw() const
{
	KGXRenderer renderer(mViewport, mRHISwapChain->getCurrentBufferView(), mDepthStencil);
	renderer.RenderFrame();

	mRHISwapChain->present();
}
}
