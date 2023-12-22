
#pragma once

#include "CommandThread.h"
#include "RenderThread.h"
#include "Timer.h"
#include "Private/Scene/KGXScene.h"

#include <memory>
#include <unordered_map>


namespace kgx::core
{
class RenderWindow;

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

    [[nodiscard]] KGXScene* getSceneThreadPtr() const;
    [[nodiscard]] RenderThread* getRenderThreadPtr() const;

    bool createRenderWindow(WinHandle windowHandle, unsigned int initialWindowWidth, unsigned int initialWindowHeight);

private:
    RenderCore();
    ~RenderCore();

    static RenderCore* mInst;
    static int mRefCount;

    std::unique_ptr<Timer> mFrameTimer;

    std::unique_ptr<KGXScene> mScene;
    std::unique_ptr<RenderThread> mRenderThread;

    std::mutex mRenderWindowMutex;
    std::unordered_map<WinHandle, std::shared_ptr<RenderWindow>> mRenderWindows;
};
}
