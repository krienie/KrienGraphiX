
#include "Simulation/SceneObjectComponent.h"

#include "Simulation/SceneObject.h"

namespace kgx
{
SceneObjectComponent::SceneObjectComponent(SceneObject *owner)
    : m_owner(owner)
{
}

SceneObject *SceneObjectComponent::getOwner() const
{
    return m_owner;
}
}
