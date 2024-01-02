
#include "KrienGraphiX/Scene/KGXMeshComponent.h"

#include "KrienGraphiX/Scene/KGXSceneObject.h"

namespace kgx
{
KGXMeshComponent::KGXMeshComponent(KGXSceneObject *owner)
    : KGXSceneObjectComponent(owner)
{
}

void KGXMeshComponent::initialize()
{
    if (mIsInitialized)
    {
        return;
    }

    //TODO(KL): Create KGXRenderGeometry



    KGXSceneObjectComponent::initialize();
}

KGXBoxMeshComponent::KGXBoxMeshComponent(KGXSceneObject* owner)
    : KGXMeshComponent(owner)
{
}

KGXMeshRenderObject* KGXBoxMeshComponent::createMeshRenderObject()
{

}
}
