
#pragma once

#include "Private/Core/CommandThread.h"
#include "Private/Core/SceneThread.h"
#include "Private/Core/RenderThread.h"

#include <memory>
#include <unordered_map>

#include "Private/Rendering/KGXShaderCache.h"

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

    [[nodiscard]] SceneThread* getSceneThreadPtr() const;
    [[nodiscard]] RenderThread* getRenderThreadPtr() const;

    bool createRenderWindow(WinHandle windowHandle, unsigned int initialWindowWidth, unsigned int initialWindowHeight);

private:
    RenderCore();
    ~RenderCore();

    static RenderCore* mInst;
    static int mRefCount;

    std::unique_ptr<SceneThread> mSceneThread;
    std::unique_ptr<RenderThread> mRenderThread;
    //CommandThread mThreadPool; Not really needed for now

    std::mutex mRenderWindowMutex;
    std::unordered_map<WinHandle, std::shared_ptr<RenderWindow>> mRenderWindows;
    rendering::KGXShaderCache mShaderCache;
};
}
