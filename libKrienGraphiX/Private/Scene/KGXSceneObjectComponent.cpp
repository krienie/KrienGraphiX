
#include "KrienGraphiX/Scene/KGXSceneObjectComponent.h"

#include "KrienGraphiX/Scene/KGXSceneObject.h"

namespace kgx
{
KGXSceneObjectComponent::KGXSceneObjectComponent(KGXSceneObject* owner)
    : mOwner(owner)
{
}

KGXSceneObject* KGXSceneObjectComponent::getOwner() const
{
    return mOwner;
}
}
