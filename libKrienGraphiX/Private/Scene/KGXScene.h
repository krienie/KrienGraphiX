
#pragma once

#include "KrienGraphiX/Core/KGXDefinitions.h"

#include <memory>
#include <mutex>

namespace kgx
{
    class KGXSceneObject;
}

namespace kgx::core
{
class KGXScene final
{
public:
    KGXScene() = default;
    ~KGXScene() = default;

    KGXScene(const KGXScene&) noexcept            = delete;
    KGXScene(KGXScene&&) noexcept                 = delete;
    KGXScene& operator=(const KGXScene&) noexcept = delete;
    KGXScene& operator=(KGXScene&&) noexcept      = delete;

    void updateScene(float deltaTime);

    void addSceneObject(const std::shared_ptr<KGXSceneObject>& sceneObject);
    void addSceneUpdateDelegate(SceneUpdateDelegate updateDelegate);

private:
    std::mutex mUpdateSceneObjectsMutex;
    std::vector<std::shared_ptr<KGXSceneObject>> mSceneObjects;

    std::mutex mUpdateDelegateMutex;
    std::vector<SceneUpdateDelegate> mSceneUpdateDelegates;
};
}
