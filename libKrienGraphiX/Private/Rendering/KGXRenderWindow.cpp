
#include "KGXRenderWindow.h"

#include "Private/RHI/RenderHardwareInterface.h"
#include "Private/Core/RenderCore.h"

using namespace kgx::core;

namespace kgx::rendering
{
KGXRenderWindow::KGXRenderWindow(WinHandle windowHandle, unsigned int width, unsigned int height)
    : mWindowHandle(windowHandle), mRHISwapChain(nullptr), mViewport{0, 0, width, height, 0.0f, 1.0f}
{
    const auto* renderThread = RenderCore::get()->getRenderThreadPtr();

    auto* rhiDevice = renderThread->getGraphicsDevicePtr();

    using namespace RHI;
    mRHISwapChain = PlatformRHI->createSwapChain(
        rhiDevice,
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

    mDepthStencil = PlatformRHI->createDepthStencilBuffer(rhiDevice, texDesc);
}

void KGXRenderWindow::draw()
{
    const auto* renderThread = RenderCore::get()->getRenderThreadPtr();

    auto* commandList = renderThread->getGraphicsCommandListPtr();

    //TODO(KL): Move this to KGXRenderer
    const auto frameRenderTarget = mRHISwapChain->getCurrentBuffer();
    RHI::PlatformRHI->beginFrame(commandList, frameRenderTarget);
    
    commandList->setViewport(mViewport);

    //TODO(KL): temporary fixed clear color
    static float lightSteelBlue[4] = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };

    auto backBufferRtv = mRHISwapChain->getCurrentBufferRTV();
    auto depthStencilView = mDepthStencil->getResourceViewByType(RHI::RHIResourceView::DSV).get();

    // Clear the back buffer and depth buffer.
	commandList->clearRenderTargetView(backBufferRtv, lightSteelBlue);
	commandList->clearDepthStencilView(depthStencilView,
                                        static_cast<RHI::RHIResourceView::DepthStencilFlags>(RHI::RHIResourceView::DepthClear | RHI::RHIResourceView::StencilClear),
                                       1.0f, 0);
	
    // Specify the buffers we are going to render to.
	commandList->setRenderTargets({backBufferRtv}, depthStencilView);

    RHI::PlatformRHI->endFrame(commandList, frameRenderTarget);

    renderThread->getCommandQueuePtr()->executeCommandList(commandList);

    mRHISwapChain->present();

    renderThread->getCommandQueuePtr()->flushQueue();
}
}
