
#pragma once

#include "KrienGraphiX/Core/KGXDefinitions.h"
#include "Private/Rendering/KGXRenderScene.h"

#include <mutex>

namespace kgx
{
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

	void updateScene(float deltaTime);

	void addMeshComponent(KGXMeshComponent* meshComponent);

	void enqueueMeshTransformUpdate(const KGXMeshComponent* meshComponent);

private:
	std::vector<KGXMeshComponent*> mMeshComponents;

	std::mutex mUpdateDelegateMutex;
	std::vector<SceneUpdateDelegate> mSceneUpdateDelegates;

	rendering::KGXRenderScene mRenderScene;

	std::vector<rendering::MeshTransformUpdateParams> mPendingMeshTransformUpdates;
};
}
