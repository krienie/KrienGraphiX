
#pragma once

#include "KrienGraphiX/Core/KGXDefinitions.h"
#include "Private/Rendering/KGXRenderScene.h"

#include <mutex>

namespace kgx
{
class KGXCameraComponent;
class KGXMeshComponent;
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

	const rendering::KGXRenderScene* getRenderScenePtr() const;

	void addSceneUpdateDelegate(SceneUpdateDelegate updateDelegate);

	bool hasActiveCamera() const;
	void setActiveCamera(KGXCameraComponent* cameraComponent);

	void addMeshComponent(KGXMeshComponent* meshComponent);

	void updateScene(float deltaTime);
	void enqueueMeshTransformUpdate(const KGXMeshComponent* meshComponent);

private:
	KGXCameraComponent* mActiveCamera = nullptr;
	std::vector<KGXMeshComponent*> mMeshComponents;

	std::mutex mUpdateDelegateMutex;
	std::vector<SceneUpdateDelegate> mSceneUpdateDelegates;

	rendering::KGXRenderScene mRenderScene;

	std::vector<rendering::MeshTransformUpdateParams> mPendingMeshTransformUpdates;
};
}
