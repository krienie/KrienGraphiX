
#include "KrienGraphiX/Core/RenderWindow.h"

#include "Private/RHI/RenderHardwareInterface.h"
#include "RenderCore.h"

namespace kgx::core
{
RenderWindow::RenderWindow(WinHandle windowHandle, unsigned int width, unsigned int height)
    : mRHISwapChain(nullptr), mViewport{0, 0, width, height, 0.0f, 1.0f}
{
    const auto* renderEngine = RenderCore::get();
    
    mRHISwapChain = RHI::PlatformRHI->createSwapChain(
        renderEngine->getGraphicsDevicePtr(),
        renderEngine->getCommandQueuePtr(),
        windowHandle,
        width,
        height,
        2); // Front- and back-buffer
}

void RenderWindow::draw()
{
    const auto* renderCore = RenderCore::get();

    auto* commandList = renderCore->getGraphicsCommandListPtr();

    //TODO(KL): Move this to KGXRenderer
    const auto frameRenderTarget = mRHISwapChain->getCurrentBuffer();
    RHI::PlatformRHI->beginFrame(commandList, frameRenderTarget.get());
    
    commandList->setViewport(mViewport);

    // Clear the back buffer and depth buffer.
	//mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	//mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	
    // Specify the buffers we are going to render to.
	//mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

    RHI::PlatformRHI->endFrame(commandList, frameRenderTarget.get());

    renderCore->getCommandQueuePtr()->executeCommandList(commandList);

    mRHISwapChain->present();

    renderCore->getCommandQueuePtr()->flushQueue();
}
}
