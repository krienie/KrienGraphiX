
#include "KGXRenderScene.h"


namespace kgx::rendering
{
void KGXRenderScene::addRenderObject(const std::shared_ptr<KGXMeshRenderObject>& renderObject)
{
	mRenderObjects.push_back(renderObject);

	renderObject->createRenderResources();
}

void KGXRenderScene::updateRenderObjectTransforms(const std::vector<MeshTransformUpdateParams>& transformUpdates)
{
	for (const MeshTransformUpdateParams& update : transformUpdates)
	{
		update.meshToUpdate->updateTransform(update.transform);
	}
}

void KGXRenderScene::updateActiveCameraMatrix(const math::Matrix4X4& viewProjection)
{
	mActiveCameraMatrix = viewProjection;
}

const math::Matrix4X4& KGXRenderScene::getActiveCameraMatrix() const
{
	return mActiveCameraMatrix;
}
}
