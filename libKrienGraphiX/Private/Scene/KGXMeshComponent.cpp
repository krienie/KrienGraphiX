
#include "KrienGraphiX/Scene/KGXMeshComponent.h"

#include "KrienGraphiX/Scene/KGXSceneObject.h"
#include "Private/Core/RenderCore.h"

#include <array>
#include <DirectXColors.h>

namespace kgx
{
KGXMeshComponent::KGXMeshComponent(KGXSceneObject *owner)
	: KGXSceneObjectComponent(owner)
{
}

bool KGXMeshComponent::initializeImpl()
{
	core::RenderCore::get()->getScenePtr()->addMeshComponent(this);

	return true;
}

void KGXMeshComponent::updateImpl([[maybe_unused]] float deltaTime)
{
	if (hasTransformChangedThisFrame())
	{
		getOwner()->getParentScene()->enqueueMeshTransformUpdate(this);
	}
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
	using namespace kgx::rendering;
	using namespace DirectX;

	KGXMeshRenderObject::RawMeshData meshData;
	meshData.vertices = 
	{
		Vertex({.Pos = XMFLOAT3(-1.0f, -1.0f, -1.0f), .Color = XMFLOAT4(Colors::White) }),
		Vertex({.Pos = XMFLOAT3(-1.0f, +1.0f, -1.0f), .Color = XMFLOAT4(Colors::Black) }),
		Vertex({.Pos = XMFLOAT3(+1.0f, +1.0f, -1.0f), .Color = XMFLOAT4(Colors::Red) }),
		Vertex({.Pos = XMFLOAT3(+1.0f, -1.0f, -1.0f), .Color = XMFLOAT4(Colors::Green) }),
		Vertex({.Pos = XMFLOAT3(-1.0f, -1.0f, +1.0f), .Color = XMFLOAT4(Colors::Blue) }),
		Vertex({.Pos = XMFLOAT3(-1.0f, +1.0f, +1.0f), .Color = XMFLOAT4(Colors::Yellow) }),
		Vertex({.Pos = XMFLOAT3(+1.0f, +1.0f, +1.0f), .Color = XMFLOAT4(Colors::Cyan) }),
		Vertex({.Pos = XMFLOAT3(+1.0f, -1.0f, +1.0f), .Color = XMFLOAT4(Colors::Magenta) })
	};

	meshData.indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	return new KGXMeshRenderObject(meshData);
}
}
