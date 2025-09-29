
#include "KrienGraphiX/Scene/KGXSceneObject.h"

#include "KGXScene.h"
#include "KrienGraphiX/Scene/KGXCameraComponent.h"
#include "KrienGraphiX/Scene/KGXMeshComponent.h"
#include "KrienGraphiX/Scene/KGXSceneObjectComponent.h"
#include "Private/Core/RenderCore.h"

namespace kgx
{
KGXSceneObject::KGXSceneObject(const std::string& name)
	: mName(name)
{
	mParentScene = core::RenderCore::get()->getScenePtr();

	//TODO(KL): Think of something better for this..
	mParentScene->addSceneUpdateDelegate([this](float deltaTime)
	{
		update(deltaTime);
	});
}

core::KGXScene* KGXSceneObject::getParentScene() const
{
	return mParentScene;
}

void KGXSceneObject::setPosition(float xPos, float yPos, float zPos)
{
	mTransform.setTranslation(xPos, yPos, zPos);
	mHasTransformChanged = true;
}

void KGXSceneObject::setRotation(float pitch, float yaw, float roll)
{
	mTransform.setRotation(pitch, yaw, roll);
	mHasTransformChanged = true;
}

void KGXSceneObject::setScale(float xScale, float yScale, float zScale)
{
	mTransform.setScale(xScale, yScale, zScale);
	mHasTransformChanged = true;
}

std::string KGXSceneObject::getName() const
{
	return mName;
}

DirectX::XMFLOAT4X4 KGXSceneObject::getWorldTransform() const
{
	return mTransform.getMatrix();
}

void KGXSceneObject::update(float deltaTime)
{
	updateImpl(deltaTime);

	for (const auto& sceneComponent : mSceneComponents)
	{
		sceneComponent->update(deltaTime);
	}

	mHasTransformChanged = false;
}

std::vector<std::shared_ptr<KGXSceneObjectComponent>> KGXSceneObject::getComponents() const
{
	return mSceneComponents;
}

void KGXSceneObject::addNewComponentInternal(KGXSceneObjectComponent* newComponent)
{
	newComponent->initialize();

	mSceneComponents.emplace_back(newComponent);
}

KGXBoxObject::KGXBoxObject(const std::string& name)
	: KGXSceneObject(name)
{
	addNewComponent<KGXBoxMeshComponent>();
}

KGXCameraObject::KGXCameraObject(const std::string& name)
	: KGXSceneObject(name)
{
	//TODO(KL): Add interface for setting camera settings
	constexpr DirectX::XMFLOAT3 eye(0, -20, -20);
	constexpr DirectX::XMFLOAT3 target(0, 0, 0);
	constexpr DirectX::XMFLOAT3 up(0, 0, 1);
	addNewComponent<KGXCameraComponent>(eye, target, up);
}
}
