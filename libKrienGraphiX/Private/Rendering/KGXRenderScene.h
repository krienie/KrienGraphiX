
#pragma once

#include <memory>
#include <vector>

#include "KGXMeshRenderObject.h"

namespace kgx::rendering
{
struct MeshTransformUpdateParams
{
	KGXMeshRenderObject* meshToUpdate;
	math::Matrix4X4 transform;
};

class KGXRenderScene
{
public:
	KGXRenderScene() = default;
	~KGXRenderScene() = default;

	void addRenderObject(const std::shared_ptr<KGXMeshRenderObject>& renderObject);
	void updateRenderObjectTransforms(const std::vector<MeshTransformUpdateParams>& transformUpdates);

private:
	std::vector<std::shared_ptr<KGXMeshRenderObject>> mRenderObjects;
};
}
