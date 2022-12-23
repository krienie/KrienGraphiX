
#include "RenderWindow.h"

#include "RenderCore.h"
#include "Private/Rendering/KGXRenderWindow.h"

using namespace kgx::rendering;

namespace kgx::core
{
RenderWindow::RenderWindow(WinHandle windowHandle, unsigned int width, unsigned int height)
    : mKGXRenderWindow(std::make_shared<KGXRenderWindow>(windowHandle, width, height))
{
    //TODO(KL): create mKGXRenderWindow on the renderthread somehow
}

WinHandle RenderWindow::getWinHandle() const
{
    return mKGXRenderWindow->getWinHandle();
}

void RenderWindow::draw() const
{
    RenderCore::get()->getRenderThreadPtr()->enqueueCommand([this]()
    {
        mKGXRenderWindow->draw();
    });
}
}
