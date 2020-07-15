
#include "Core/RenderCollection.h"

#include <memory>
#include <vector>

#include "Simulation/CameraComponent.h"
#include "Simulation/MeshComponent.h"
#include "Simulation/Scene.h"
#include "Simulation/SceneObject.h"

namespace kgx
{
    RenderCollection::RenderCollection()
        : m_culledObjects(), m_cameraComponent(nullptr)
    {
    }

    void RenderCollection::cullView(const Scene *scene)
    {
        const std::vector<SceneObject*> *sceneObjects = scene->getSceneObjects();

        // TODO(KL): cull scene according to the current camera view

        for (const auto &sceneObject : *sceneObjects)
        {
            m_culledObjects.push_back(CulledObject{sceneObject->getName(), sceneObject->getModelMatrix(), sceneObject->getNormalMatrix()});
            for (const auto &component : sceneObject->getComponents())
            {
                const auto* meshComp = dynamic_cast<MeshComponent*>(component.get());
                if (meshComp)
                {
                    m_culledObjects.back().meshComponents.push_back(std::static_pointer_cast<MeshComponent>(component));
                    continue;
                }

                const auto* cameraComp = dynamic_cast<CameraComponent*>(component.get());
                if (cameraComp && !m_cameraComponent)
                {
                    // For now, just take the first camera we find.
                    m_cameraComponent = std::static_pointer_cast<CameraComponent>(component);
                }
            }
        }
    }

    const std::vector<RenderCollection::CulledObject>* RenderCollection::getCulledObjects() const
    {
        return &m_culledObjects;
    }

    std::shared_ptr<CameraComponent> RenderCollection::getCamera() const
    {
        return m_cameraComponent;
    }
}
