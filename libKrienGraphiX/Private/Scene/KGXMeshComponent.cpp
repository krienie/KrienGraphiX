
#include "KrienGraphiX/Scene/KGXMeshComponent.h"

#include "KrienGraphiX/Scene/KGXSceneObject.h"
#include "Private/Core/RenderCore.h"

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

    core::RenderCore::get()->getScenePtr()->addMeshComponent(this);

    KGXSceneObjectComponent::initialize();
}

std::shared_ptr<rendering::KGXMeshRenderObject> KGXMeshComponent::createMeshRenderObject()
{
    mMeshRenderObject = std::shared_ptr<rendering::KGXMeshRenderObject>(createMeshRenderObjectInternal());

    return mMeshRenderObject;
}

KGXBoxMeshComponent::KGXBoxMeshComponent(KGXSceneObject* owner)
    : KGXMeshComponent(owner)
{
}

rendering::KGXMeshRenderObject* KGXBoxMeshComponent::createMeshRenderObjectInternal()
{
    return new rendering::KGXMeshRenderObject();
}
}
