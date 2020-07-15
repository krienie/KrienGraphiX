
#pragma once

#include <memory>
#include <string>
#include <vector>

#include <DirectXMath.h>

#include "Simulation/MeshComponent.h"

namespace kgx
{
class Scene;
class CameraComponent;

class RenderCollection final
{
public:
    struct CulledObject
    {
        std::string name;
        DirectX::XMFLOAT4X4 modelMatrix;
        DirectX::XMFLOAT4X4 normalMatrix;
        std::vector<std::shared_ptr<MeshComponent>> meshComponents;
    };

    RenderCollection();

    void cullView(const Scene *scene);

    [[nodiscard]]
    const std::vector<CulledObject>* getCulledObjects() const;
    [[nodiscard]]
    std::shared_ptr<CameraComponent> getCamera() const;

private:
    std::vector<CulledObject> m_culledObjects;
    std::shared_ptr<CameraComponent> m_cameraComponent;
};
}
