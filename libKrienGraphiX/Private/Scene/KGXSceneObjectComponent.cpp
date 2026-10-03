
#include "KrienGraphiX/Scene/KGXSceneObjectComponent.h"

#include "KrienGraphiX/Scene/KGXSceneObject.h"

#include <cassert>

#include "KGXScene.h"

namespace kgx
{
KGXSceneObjectComponent::KGXSceneObjectComponent(KGXSceneObject* owner)
	: mOwner(owner)
{
	assert(mOwner);
}

KGXSceneObject* KGXSceneObjectComponent::getOwner() const
{
	return mOwner;
}

void KGXSceneObjectComponent::initialize()
{
	if (mIsInitialized)
	{
		return;
	}

	mIsInitialized = initializeImpl();
}

void KGXSceneObjectComponent::update(float deltaTime)
{
	updateImpl(deltaTime);

	mHasTransformChanged = false;
}

void KGXSceneObjectComponent::setPosition(const math::Vector3& position)
{
	mTransform.setTranslation(position);
	mHasTransformChanged = true;
}

void KGXSceneObjectComponent::setRotation(const math::Quaternion& rotation)
{
	mTransform.setRotation(rotation);
	mHasTransformChanged = true;
}

void KGXSceneObjectComponent::setScale(const math::Vector3& scale)
{
	mTransform.setScale(scale);
	mHasTransformChanged = true;
}

math::Matrix4X4 KGXSceneObjectComponent::getRelativeTransform() const
{
	return mTransform.getMatrix();
}

math::Matrix4X4 KGXSceneObjectComponent::getWorldTransform() const
{
	const math::Matrix4X4 parentTransform = mOwner->getWorldTransform();
	const math::Matrix4X4 componentTransform = getRelativeTransform();

	return parentTransform * componentTransform;
}

bool KGXSceneObjectComponent::hasTransformChangedThisFrame() const
{
	return mHasTransformChanged || mOwner->hasTransformChangedThisFrame();
}
}
