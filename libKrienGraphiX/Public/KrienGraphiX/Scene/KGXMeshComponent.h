
#pragma once

#include <memory>

#include "KGXSceneObjectComponent.h"
#include "KrienGraphiX/Core/KGXDefinitions.h"

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

	std::shared_ptr<rendering::KGXMeshRenderObject> createMeshRenderObject();

	[[nodiscard]] rendering::KGXMeshRenderObject* getMeshRenderObject() const { return mMeshRenderObject.get(); }

	//void setMaterial(const Material& material);
	//Material getMaterial() const;

private:
	bool initializeImpl() override;
	void updateImpl([[maybe_unused]] float deltaTime) override;

	virtual rendering::KGXMeshRenderObject* createMeshRenderObjectInternal() = 0;
	//Material m_material;

	std::shared_ptr<rendering::KGXMeshRenderObject> mMeshRenderObject;
};

class KGXBoxMeshComponent : public KGXMeshComponent
{
public:
	KGXBoxMeshComponent(KGXSceneObject *owner);
	~KGXBoxMeshComponent() override = default;

private:
	rendering::KGXMeshRenderObject* createMeshRenderObjectInternal() override;
};

class KGXCustomMeshComponent : public KGXMeshComponent
{
public:
	KGXCustomMeshComponent(KGXSceneObject *owner, const RawMeshData& meshData);
	~KGXCustomMeshComponent() override = default;

private:
	rendering::KGXMeshRenderObject* createMeshRenderObjectInternal() override;

	RawMeshData mMeshData;
};
}
