
#include "RenderWindow.h"

#include "RenderCore.h"
#include "Private/Rendering/KGXRenderWindow.h"

using namespace kgx::rendering;

namespace kgx::core
{
RenderWindow::RenderWindow(SDL_Window* window, unsigned int width, unsigned int height)
	: mKGXRenderWindow(std::make_shared<KGXRenderWindow>(window, width, height))
{
}

SDL_Window* RenderWindow::getWinHandle() const
{
	return mKGXRenderWindow->getWinHandle();
}

void RenderWindow::draw()
{
	bool expected = false;
	if (mIsWindowBeingRendered.compare_exchange_strong(expected, true))
	{
		auto renderThread = RenderCore::get()->getRenderThreadPtr();
		renderThread->enqueueCommand([this, renderThread]()
		{
			renderThread->nextFrame();
			mKGXRenderWindow->draw();
			mIsWindowBeingRendered = false;
		});
	}
}
}
