
#include "KGXRenderWindow.h"

#include "KGXRenderer.h"
#include "Private/RHI/RenderHardwareInterface.h"
#include "Private/Core/RenderCore.h"

using namespace kgx::core;

namespace kgx::rendering
{
KGXRenderWindow::KGXRenderWindow(WinHandle windowHandle, unsigned int width, unsigned int height)
	: mWindowHandle(windowHandle), mRHISwapChain(nullptr), mViewport{0, 0, width, height, 0.0f, 1.0f}
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

	const RHITexture2DDescriptor texDesc =
		{
		// depth clear = 1.0f, stencil clear = 0
		RHIClearValue{.depthClear = RHIClearValue::DepthClear{1.0f, 0}},
		RHIPixelFormat::D24_unorm_S8_uint,
		width,
		height,
		1, 1,
		RHIResource::DepthStencil
		};

	//TODO(KL): Create a global rendertarget pool where this one comes out of and move it into KGXRenderer.
	mDepthStencil = PlatformRHI->createDepthStencilBuffer(texDesc);

	constexpr bool isShaderVisible = false;
	mDSV = PlatformRHI->createResourceView(RHIResourceView::Type::DSV, mDepthStencil, isShaderVisible);
}

void KGXRenderWindow::draw()
{
	const auto* renderThread = RenderCore::get()->getRenderThreadPtr();

	KGXRenderer renderer(mViewport, *mRHISwapChain->getCurrentBufferView(), *mDSV);
	renderer.RenderFrame();

	mRHISwapChain->present();

	//TODO(KL): Niet nodig?
	renderThread->getCommandQueuePtr()->flushQueue();
}
}
