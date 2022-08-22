
#include "KrienGraphiX/Core/RenderWindow.h"

#include "Private/RHI/RenderHardwareInterface.h"
#include "RenderCore.h"

namespace kgx::core
{
RenderWindow::RenderWindow(WinHandle windowHandle, unsigned int width, unsigned int height)
    : mRHISwapChain(nullptr)
{
    const auto* renderEngine = RenderCore::get();
    
    mRHISwapChain = renderEngine->getRHI()->createSwapChain(
        renderEngine->getGraphicsDevicePtr(),
        renderEngine->getGraphicsCommandQueuePtr(),
        windowHandle,
        width,
        height,
        2); // Front- and back-buffer

    mViewport = {0, 0, width, height, 0.0f, 1.0f};
}

void RenderWindow::draw()
{
    const auto* renderCore = RenderCore::get();

    //TODO(KL): Move this to KGXRenderer
    renderCore->getRHI()->beginFrame(renderCore->getGraphicsCommandQueuePtr());

    auto* commandList = renderCore->getGraphicsCommandListPtr();

    commandList->reset(renderCore->getGraphicsCommandQueuePtr());
    
    commandList->setViewport(mViewport);



    renderCore->getRHI()->endFrame();
}
}
