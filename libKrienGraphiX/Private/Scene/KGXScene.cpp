
#include "KGXScene.h"

#include <utility>

#include "KrienGraphiX/Scene/KGXSceneObject.h"
#include "Private/Core/RenderCore.h"

namespace kgx::core
{
void KGXScene::updateScene(float deltaTime)
{
    {
        std::lock_guard lock(mUpdateDelegateMutex);
        for (const auto& updateDelegate : mSceneUpdateDelegates)
        {
            updateDelegate(deltaTime);
        }
    }

    {
        std::lock_guard lock(mUpdateSceneObjectsMutex);
        for (const auto& sceneObject : mSceneObjects)
        {
            sceneObject->update(deltaTime);
        }
    }
}

void KGXScene::addSceneObject(const std::shared_ptr<KGXSceneObject>& sceneObject)
{
    std::lock_guard lock(mUpdateSceneObjectsMutex);
    mSceneObjects.emplace_back(sceneObject);

    //RenderCore::get()->getRenderThreadPtr()->enqueueCommand([this, sceneObject]()
    //{
    //    mRenderScene.addRenderObject(sceneObject);
    //});
}

void KGXScene::addSceneUpdateDelegate(SceneUpdateDelegate updateDelegate)
{
    if (updateDelegate)
    {
        std::lock_guard lock(mUpdateDelegateMutex);
        mSceneUpdateDelegates.push_back(std::move(updateDelegate));
    }
}
}
