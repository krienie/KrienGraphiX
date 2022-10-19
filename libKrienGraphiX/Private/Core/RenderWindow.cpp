
#include "KrienGraphiX/Core/RenderWindow.h"

#include "Private/RHI/RenderHardwareInterface.h"
#include "RenderCore.h"

namespace kgx::core
{
RenderWindow::RenderWindow(WinHandle windowHandle, unsigned int width, unsigned int height)
    : mRHISwapChain(nullptr), mViewport{0, 0, width, height, 0.0f, 1.0f}
{
    const auto* renderEngine = RenderCore::get();

    auto* rhiDevice = renderEngine->getGraphicsDevicePtr();

    using namespace RHI;
    mRHISwapChain = PlatformRHI->createSwapChain(
        rhiDevice,
        renderEngine->getCommandQueuePtr(),
        windowHandle,
        width,
        height,
        2); // Front- and back-buffer


    const RHITexture2DDescriptor texDesc =
        {
        RHIClearValue{1.0, 0, 0, 0},
        RHIPixelFormat::D24_unorm_S8_uint,
        width,
        height,
        1, 1,
        RHIResource::DepthStencil
        };

    mDepthStencil = PlatformRHI->createDepthStencilBuffer(rhiDevice, texDesc);
}

void RenderWindow::draw()
{
    const auto* renderCore = RenderCore::get();

    auto* commandList = renderCore->getGraphicsCommandListPtr();

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

    renderCore->getCommandQueuePtr()->executeCommandList(commandList);

    mRHISwapChain->present();

    renderCore->getCommandQueuePtr()->flushQueue();
}
}
