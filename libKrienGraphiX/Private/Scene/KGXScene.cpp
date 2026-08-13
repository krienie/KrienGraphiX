
#include "KGXScene.h"

#include <utility>

#include "KrienGraphiX/Scene/KGXMeshComponent.h"
#include "KrienGraphiX/Scene/KGXSceneObject.h"
#include "Private/Core/RenderCore.h"

namespace kgx::core
{
rendering::KGXRenderScene* KGXScene::getRenderScenePtr()
{
	return &mRenderScene;
}

void KGXScene::setSceneUpdateDelegate(SceneUpdateDelegate updateDelegate)
{
	if (updateDelegate)
	{
		std::lock_guard lock(mUpdateDelegateMutex);
		mSceneUpdateDelegate = std::move(updateDelegate);
	}
}

void KGXScene::registerObjectUpdate(SceneUpdateDelegate updateDelegate)
{
	if (updateDelegate)
	{
		std::lock_guard lock(mUpdateDelegateMutex);
		mObjectUpdateDelegates.push_back(std::move(updateDelegate));
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

	gRenderThread->enqueueCommand([this, newMeshRenderObject]()
	{
		mRenderScene.addRenderObject(newMeshRenderObject);
	});
}

void KGXScene::updateScene(float deltaTime)
{
	{
		std::lock_guard lock(mUpdateDelegateMutex);
		if (mSceneUpdateDelegate)
		{
			mSceneUpdateDelegate(deltaTime);
		}
	}

	if (!mPendingMeshTransformUpdates.empty())
	{
		gRenderThread->enqueueCommand([this, localPendingMeshTransformUpdates = std::move(mPendingMeshTransformUpdates)]()
		{
			mRenderScene.updateRenderObjectTransforms(localPendingMeshTransformUpdates);
		});
	}

	if (mActiveCamera)
	{
		gRenderThread->enqueueCommand([this, localViewProjection = mActiveCamera->getViewProjMatrix()]()
		{
			mRenderScene.updateActiveCameraMatrix(localViewProjection);
		});
	}

	{
		std::lock_guard lock(mUpdateDelegateMutex);
		for (auto& objectUpdate : mObjectUpdateDelegates)
		{
			objectUpdate(deltaTime);
		}
	}
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
