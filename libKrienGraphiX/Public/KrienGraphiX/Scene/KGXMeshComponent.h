
#pragma once

#include "KGXSceneObjectComponent.h"

namespace kgx
{
class KGXSceneObject;
class KGXMeshRenderObject;

class MeshBuffer;

class KGXMeshComponent : public KGXSceneObjectComponent
{
public:
    KGXMeshComponent(KGXSceneObject *owner);
    ~KGXMeshComponent() override = default;

    void initialize() override;
    virtual KGXMeshRenderObject* createMeshRenderObject() = 0;
    
    //void setMaterial(const Material& material);
    //Material getMaterial() const;

private:
    //Material m_material;
};

class KGXBoxMeshComponent : public KGXMeshComponent
{
public:
    KGXBoxMeshComponent(KGXSceneObject *owner);
    ~KGXBoxMeshComponent() override = default;

    //void initialize() override;
    virtual KGXMeshRenderObject* createMeshRenderObject();
};
}
