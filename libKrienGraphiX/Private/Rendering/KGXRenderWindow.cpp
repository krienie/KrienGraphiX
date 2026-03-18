
#include "KGXRenderWindow.h"

#include "KGXRenderer.h"
#include "Private/RHI/RenderHardwareInterface.h"
#include "Private/Core/RenderCore.h"

using namespace kgx::core;

namespace kgx::rendering
{
KGXRenderWindow::KGXRenderWindow(SDL_Window* window, unsigned int width, unsigned int height)
	: mWindowHandle(window), mRHISwapChain(nullptr)
{
	const auto* renderThread = RenderCore::get()->getRenderThreadPtr();

	//TODO(KL): Integrate the swapchain into KGXRenderWindow? or KGXViewport? as an interface
	using namespace RHI;
	mRHISwapChain = PlatformRHI->createSwapChain(
		renderThread->getCommandQueuePtr(),
		mWindowHandle,
		width,
		height,
		2); // Front- and back-buffer

	mViewport =
	{
		.topLeftX = 0,
		.topLeftY = 0,
		.width = width,
		.height = height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	const RHITexture2DDescriptor texDesc =
	{
		// depth clear = 1.0f, stencil clear = 0
		.clearValue = RHIClearValue{.depthClear = RHIClearValue::DepthClear{1.0f, 0}},
		.pixelFormat = RHIPixelFormat::D24_unorm_S8_uint,
		.width = width,
		.height = height,
		.numMips = 1,
		.numSamples = 1,
		.flags = RHIResource::DepthStencil
	};

#ifndef __APPLE__
	//TODO(KL): Create a global rendertarget pool where this one comes out of and move it into KGXRenderer.
	mDepthStencil = PlatformRHI->createDepthStencilBuffer(texDesc);

	constexpr bool isShaderVisible = false;
	mDSV = PlatformRHI->createResourceView(RHIResourceView::Type::DSV, mDepthStencil, isShaderVisible);
#endif
}

void KGXRenderWindow::draw() const
{
	KGXRenderer renderer(mViewport, *mRHISwapChain->getCurrentBufferView(), *mDSV);
	renderer.RenderFrame();

	mRHISwapChain->present();
}
}
