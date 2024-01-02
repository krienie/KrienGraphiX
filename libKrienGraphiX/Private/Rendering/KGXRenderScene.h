
#pragma once

#include <memory>
#include <vector>

#include "KGXRenderGeometry.h"
#include "KGXMeshRenderObject.h"

namespace kgx::rendering
{
class KGXRenderScene
{
public:
    KGXRenderScene() = default;
    ~KGXRenderScene() = default;

    void addRenderObject(const std::weak_ptr<KGXMeshRenderObject>& renderObject);

private:
    std::vector<std::weak_ptr<KGXMeshRenderObject>> mRenderObjects;
};
}
