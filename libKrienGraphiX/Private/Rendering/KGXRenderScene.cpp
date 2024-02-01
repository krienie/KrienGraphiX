
#include "KGXRenderScene.h"


namespace kgx::rendering
{
void KGXRenderScene::addRenderObject(const std::shared_ptr<KGXMeshRenderObject>& renderObject)
{
    mRenderObjects.push_back(renderObject);

    renderObject->createRenderResources();
}
}
