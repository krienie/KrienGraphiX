
#include "SceneThread.h"

#include "CommandThread.h"

#include <utility>

namespace kgx::core
{
SceneThread::SceneThread()
    : mFrameTimer(nullptr)
{
}

SceneThread::~SceneThread()
{
    // Make sure the frame time is stopped before we destruct everything else
    if (mFrameTimer)
    {
        mFrameTimer->stop();
    }
}

void SceneThread::start()
{
    constexpr int timeIntervalMilli = 16; // +- 60 FPS
    mFrameTimer = std::make_unique<Timer>(timeIntervalMilli, [this](float deltaTime)
    {
        updateScene(deltaTime);

        {
            std::lock_guard lock(mPostUpdateDelegateMutex);
            for (const auto& postUpdateDelegate : mPostSceneUpdateDelegates)
            {
                postUpdateDelegate();
            }
        }
    });
}

void SceneThread::updateScene(float deltaTime)
{
    {
        std::lock_guard lock(mUpdateDelegateMutex);
        for (const auto& updateDelegate : mSceneUpdateDelegates)
        {
            updateDelegate(deltaTime);
        }
    }

    //TODO(KL): update scene
}

void SceneThread::addSceneUpdateDelegate(SceneUpdateDelegate updateDelegate)
{
    if (updateDelegate)
    {
        std::lock_guard lock(mUpdateDelegateMutex);
        mSceneUpdateDelegates.push_back(std::move(updateDelegate));
    }
}

void SceneThread::addPostSceneUpdateDelegate(PostSceneUpdateDelegate postUpdateDelegate)
{
    if (postUpdateDelegate)
    {
        std::lock_guard lock(mPostUpdateDelegateMutex);
        mPostSceneUpdateDelegates.push_back(std::move(postUpdateDelegate));
    }
}
}
