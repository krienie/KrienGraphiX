
#pragma once

#include "Private/Core/CommandThread.h"
#include "Private/Core/RenderThread.h"

#include <memory>

#include "Timer.h"

namespace kgx::core
{
class RenderCore final
{
public:
    //TODO(KL): Maybe just make this a global variable
    static RenderCore* get();
    static void startup();
    static void shutdown();

    RenderCore(const RenderCore&) noexcept            = delete;
    RenderCore& operator=(const RenderCore&) noexcept = delete;
    RenderCore(RenderCore&&) noexcept                 = delete;
    RenderCore& operator=(RenderCore&&) noexcept      = delete;

    //[[nodiscard]] SceneThread* getSceneThreadPtr() const;
    [[nodiscard]] RenderThread* getRenderThreadPtr() const;

private:
    RenderCore();
    ~RenderCore() = default;

    static RenderCore* mInst;
    static int mRefCount;

    std::unique_ptr<Timer> mFrameTimer;
    CommandThread mSceneThread;
    std::unique_ptr<RenderThread> mRenderThread;
    //CommandThread mThreadPool; Not really needed for now
};
}
