
#pragma once

#include <memory>

#include "KGXSceneObjectComponent.h"

namespace kgx
{
class KGXSceneObject;

namespace rendering
{
class KGXMeshRenderObject;
}

class KGXMeshComponent : public KGXSceneObjectComponent
{
public:
    KGXMeshComponent(KGXSceneObject *owner);
    ~KGXMeshComponent() override = default;

    void initialize() override;
    std::shared_ptr<rendering::KGXMeshRenderObject> createMeshRenderObject();
    
    //void setMaterial(const Material& material);
    //Material getMaterial() const;

private:
    virtual rendering::KGXMeshRenderObject* createMeshRenderObjectInternal() = 0;
    //Material m_material;

    std::shared_ptr<rendering::KGXMeshRenderObject> mMeshRenderObject;
};

class KGXBoxMeshComponent : public KGXMeshComponent
{
public:
    KGXBoxMeshComponent(KGXSceneObject *owner);
    ~KGXBoxMeshComponent() override = default;

    //void initialize() override;

private:
    rendering::KGXMeshRenderObject* createMeshRenderObjectInternal() override;
};
}
