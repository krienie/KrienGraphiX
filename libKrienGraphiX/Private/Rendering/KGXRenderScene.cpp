
#include "KGXRenderScene.h"


namespace kgx::rendering
{
void KGXRenderScene::addRenderObject(const std::weak_ptr<KGXMeshRenderObject>& renderObject)
{
    mRenderObjects.push_back(renderObject);

    if (auto objPtr = renderObject.lock())
    {
        objPtr->createRenderResources();
    }
}
}
