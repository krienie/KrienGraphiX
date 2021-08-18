
#include "RcRenderWindow.h"

#include "Private/RHI/RenderHardwareInterface.h"
#include "RenderCore.h"

namespace kgx::core
{
RcRenderWindow::RcRenderWindow(WinHandle windowHandle, unsigned int width, unsigned int height)
    : mRHISwapChain(nullptr)
{
    auto * renderEngine = RenderCore::get();
    
    mRHISwapChain = renderEngine->getRHI()->createSwapChain(
        renderEngine->getGraphicsDevicePtr(),
        renderEngine->getGraphicsCommandQueuePtr(),
        windowHandle,
        width,
        height,
        2); // Front- and back-buffer
}
}
