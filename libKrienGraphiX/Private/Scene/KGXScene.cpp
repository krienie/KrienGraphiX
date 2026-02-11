
#include "KGXScene.h"

#include <utility>

#include "KrienGraphiX/Scene/KGXMeshComponent.h"
#include "KrienGraphiX/Scene/KGXSceneObject.h"
#include "Private/Core/RenderCore.h"

namespace kgx::core
{
const rendering::KGXRenderScene* KGXScene::getRenderScenePtr() const
{
	return &mRenderScene;
}

void KGXScene::addSceneUpdateDelegate(SceneUpdateDelegate updateDelegate)
{
	//TODO(KL): Implement update priority

	if (updateDelegate)
	{
		std::lock_guard lock(mUpdateDelegateMutex);
		mSceneUpdateDelegates.push_back(std::move(updateDelegate));
	}
}

bool KGXScene::hasActiveCamera() const
{
	return mActiveCamera != nullptr;
}

void KGXScene::setActiveCamera(KGXCameraComponent* cameraComponent)
{
	mActiveCamera = cameraComponent;
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

void KGXScene::updateScene(float deltaTime)
{
	{
		std::lock_guard lock(mUpdateDelegateMutex);
		for (const auto& updateDelegate : mSceneUpdateDelegates)
		{
			updateDelegate(deltaTime);
		}
	}

	if (!mPendingMeshTransformUpdates.empty())
	{
		RenderCore::get()->getRenderThreadPtr()->enqueueCommand([this, localPendingMeshTransformUpdates = std::move(mPendingMeshTransformUpdates)]()
		{
			mRenderScene.updateRenderObjectTransforms(localPendingMeshTransformUpdates);
		});
	}

	if (mActiveCamera)
	{
		RenderCore::get()->getRenderThreadPtr()->enqueueCommand([this, localViewProjection = mActiveCamera->getViewProjMatrix()]()
		{
			mRenderScene.updateActiveCameraMatrix(localViewProjection);
		});
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

void KGXScene::enqueueMeshTransformUpdate(const KGXMeshComponent* meshComponent)
{
	rendering::MeshTransformUpdateParams updateParams =
	{
		.meshToUpdate = meshComponent->getMeshRenderObject(),
		.transform = meshComponent->getWorldTransform()
	};

	mPendingMeshTransformUpdates.push_back(updateParams);
}
}
