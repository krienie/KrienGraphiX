
#pragma once

#include "KrienGraphiX/Core/KGXDefinitions.h"

#include <functional>
#include <memory>
#include <mutex>

#include "Timer.h"

namespace kgx::core
{
class CommandThread;

class SceneThread final
{
    public:
        using PostSceneUpdateDelegate = std::function<void()>;

        SceneThread();
        ~SceneThread();

        SceneThread(const SceneThread&) noexcept            = delete;
        SceneThread(SceneThread&&) noexcept                 = delete;
        SceneThread& operator=(const SceneThread&) noexcept = delete;
        SceneThread& operator=(SceneThread&&) noexcept      = delete;

        void start();

        void updateScene(float deltaTime);

        void addSceneUpdateDelegate(SceneUpdateDelegate updateDelegate);
        void addPostSceneUpdateDelegate(PostSceneUpdateDelegate postUpdateDelegate);

    private:
        std::unique_ptr<Timer> mFrameTimer;

        std::mutex mPostUpdateDelegateMutex;
        std::vector<PostSceneUpdateDelegate> mPostSceneUpdateDelegates;

        std::mutex mUpdateDelegateMutex;
        std::vector<SceneUpdateDelegate> mSceneUpdateDelegates;

        //TODO(KL): Add scene member here
};
}
