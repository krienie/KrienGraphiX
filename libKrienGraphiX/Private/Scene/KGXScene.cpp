
#include "KGXScene.h"

#include <utility>

#include "KrienGraphiX/Scene/KGXMeshComponent.h"
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

    //TODO(KL): Implement update tick for SceneObjects
    //{
    //    std::lock_guard lock(mUpdateMeshComponentsMutex);
    //    for (const auto& sceneObject : mMeshComponents)
    //    {
    //        sceneObject->update(deltaTime);
    //    }
    //}
}

void KGXScene::addMeshComponent(KGXMeshComponent* meshComponent)
{
    mMeshComponents.push_back(meshComponent);

    std::shared_ptr<rendering::KGXMeshRenderObject> newMeshRenderObject = meshComponent->createMeshRenderObject();

    RenderCore::get()->getRenderThreadPtr()->enqueueCommand([this, newMeshRenderObject]()
    {
        mRenderScene.addRenderObject(newMeshRenderObject);
    });
}

const rendering::KGXRenderScene* KGXScene::getRenderScenePtr() const
{
    return &mRenderScene;
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
