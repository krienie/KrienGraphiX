
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

	mParentScene->registerObjectUpdate([this](float deltaTime)
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

const math::Transform& KGXSceneObject::getTransform() const
{
	return mTransform;
}

math::Matrix4X4 KGXSceneObject::getWorldTransform() const
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
	constexpr math::Vector3 eye(5, 5, 10);
	constexpr math::Vector3 target(0, 0, 0);
	constexpr math::Vector3 up(0, 0, 1);
	mCameraComponent = addNewComponent<KGXCameraComponent>(eye, target, up);
}

KGXCameraComponent* KGXCameraObject::getCamera() const
{
	return mCameraComponent;
}
}
