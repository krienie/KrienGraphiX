
#include "RenderCore.h"

#include "RenderWindow.h"

#include <memory>

#ifdef _WIN32
#ifdef _DEBUG
#include <Windows.h>
#endif
#endif

namespace kgx::core
{
RenderCore* RenderCore::mInst = nullptr;
int RenderCore::mRefCount = 0;

RenderCore* RenderCore::get()
{
	return mInst;
}

void RenderCore::startup()
{
	++mRefCount;

	if (!mInst)
	{
		mInst = new RenderCore();
	}
}

void RenderCore::shutdown()
{
	--mRefCount;

	if (mRefCount == 0)
	{
		delete mInst;
	}
}

KGXScene* RenderCore::getScenePtr() const
{
	return mScene.get();
}

RenderThread* RenderCore::getRenderThreadPtr() const
{
	return mRenderThread.get();
}

bool RenderCore::createRenderWindow(SDL_Window* window, unsigned initialWindowWidth, unsigned initialWindowHeight)
{
	std::lock_guard lock(mRenderWindowMutex);

	if (const auto it = mRenderWindows.find(window); it != mRenderWindows.end())
	{
		// RenderWindow with handle windowHandle already exists. Just exit here
		//TODO(KL): log error
		return false;
	}

	//TODO(KL): Perhaps only allow one active RenderWindow for now
	mRenderWindows[window] = std::make_shared<RenderWindow>(window, initialWindowWidth, initialWindowHeight);
	return true;
}

RenderCore::RenderCore()
{
	mScene = std::make_unique<KGXScene>();
	mRenderThread = std::make_unique<RenderThread>();

#ifdef WIN32
#ifdef _DEBUG
		// open console when compiling for debugging
		AllocConsole();
		AttachConsole(GetCurrentProcessId());

#pragma warning(push)
#pragma warning(disable : 4996) // 'freopen': This function or variable may be unsafe
		freopen( "CON", "w", stdout );
		freopen( "CON", "w", stderr );
#pragma warning(pop)
#endif
#endif

	constexpr int targetFPS = 60;
	mFrameTimer = std::make_unique<FrameTimer>(targetFPS, [this](float deltaTime)
	{
		mScene->updateScene(deltaTime);

		{
			std::lock_guard lock(mRenderWindowMutex);
			for (const auto& [_, renderWindow]: mRenderWindows)
			{
				renderWindow->draw();
			}
		}
	});
}

RenderCore::~RenderCore()
{
	// Make sure the frame time is stopped before we destruct everything else
	mFrameTimer.reset();
	mRenderThread.reset();
}
}
