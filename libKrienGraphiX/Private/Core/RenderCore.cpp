
#include "RenderCore.h"

#include "RenderWindow.h"

#ifdef WIN32
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

    if ( !mInst )
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

SceneThread* RenderCore::getSceneThreadPtr() const
{
    return mSceneThread.get();
}

RenderThread* RenderCore::getRenderThreadPtr() const
{
    return mRenderThread.get();
}

bool RenderCore::createRenderWindow(WinHandle windowHandle, unsigned initialWindowWidth, unsigned initialWindowHeight)
{
    std::lock_guard lock(mRenderWindowMutex);

    if (const auto it = mRenderWindows.find(windowHandle); it != mRenderWindows.end())
    {
        // RenderWindow with handle windowHandle already exists. Just exit here
        //TODO(KL): log error
        return false;
    }

    mRenderWindows[windowHandle] = std::make_shared<RenderWindow>(windowHandle, initialWindowWidth, initialWindowHeight);
    return true;
}

RenderCore::RenderCore()
    : mSceneThread(std::make_unique<SceneThread>()), mRenderThread(std::make_unique<RenderThread>())
{
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

    mSceneThread->addPostSceneUpdateDelegate([this]()
    {
        std::lock_guard lock(mRenderWindowMutex);
        for (auto [_, renderWindow]: mRenderWindows)
        {
            renderWindow->draw();
        }
    });

    mSceneThread->start();
}

RenderCore::~RenderCore()
{
    // Make sure the SceneThread is stopped before we destruct everything else
    mSceneThread.reset();
}
}
