
#include "RenderCore.h"

#include <iostream>
#include <memory>

#include "RenderWindow.h"

#include "KrienGraphiX/Core/Logging.h"

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

bool RenderCore::createRenderWindow(SDL_Window* window, unsigned initialWindowWidth, unsigned initialWindowHeight)
{
	std::lock_guard lock(mRenderWindowMutex);

	if (mRenderWindow)
	{
		KGXLOG_WARN("Render Window already created.");
		return false;
	}

	mRenderWindow = std::make_shared<RenderWindow>(window, initialWindowWidth, initialWindowHeight);
	return mRenderWindow != nullptr;
}

RenderCore::RenderCore()
{
	mLogger = std::make_unique<Logger>();
	KGXLOG_INFO("Initializing RenderCore...");

	mScene = std::make_unique<KGXScene>();
	gRenderThread = std::make_unique<RenderThread>();

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
	KGXLOG_INFO("TargetFPS: {}", targetFPS);

	mFrameTimer = std::make_unique<FrameTimer>(targetFPS, [this](float deltaTime)
	{
		mScene->updateScene(deltaTime);

		{
			std::scoped_lock lock(mRenderWindowMutex);
			if (mRenderWindow)
			{
				mRenderWindow->draw();
			}
		}
	});

	KGXLOG_INFO("RenderCore fully initialized.");
}

RenderCore::~RenderCore()
{
	// Make sure the frame time is stopped before we destruct everything else
	mFrameTimer.reset();

	gRenderThread->flush();
	gRenderThread->shutdown();
	gRenderThread.reset();

	mLogger.reset();
}
}
