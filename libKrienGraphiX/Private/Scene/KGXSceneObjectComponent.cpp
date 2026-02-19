
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

void KGXSceneObjectComponent::setPosition(float xPos, float yPos, float zPos)
{
	mTransform.setTranslation(xPos, yPos, zPos);
	mHasTransformChanged = true;
}

void KGXSceneObjectComponent::setRotation(float pitch, float yaw, float roll)
{
	mTransform.setRotation(pitch, yaw, roll);
	mHasTransformChanged = true;
}

void KGXSceneObjectComponent::setScale(float xScale, float yScale, float zScale)
{
	mTransform.setScale(xScale, yScale, zScale);
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

	//TODO(KL): Double check the order of this
	return parentTransform * componentTransform;
}

bool KGXSceneObjectComponent::hasTransformChangedThisFrame() const
{
	return mHasTransformChanged || mOwner->hasTransformChangedThisFrame();
}
}
