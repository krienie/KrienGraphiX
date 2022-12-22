
#include "RenderCore.h"

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

RenderThread* RenderCore::getRenderThreadPtr() const
{
    return mRenderThread.get();
}

RenderCore::RenderCore()
    : mSceneThread(1), mRenderThread(std::make_unique<RenderThread>())
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

    mFrameTimer = std::make_unique<Timer>(500, []()
    {
        //static int frameCounter = 0;


    });
}
}
