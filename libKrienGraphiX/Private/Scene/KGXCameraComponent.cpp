
#include "KrienGraphiX/Scene/KGXCameraComponent.h"

#include "KrienGraphiX/Scene/KGXSceneObject.h"
#include "Private/Core/RenderCore.h"

namespace kgx
{
KGXCameraComponent::KGXCameraComponent(KGXSceneObject* owner, const math::Vector3& eye, const math::Vector3& target, const math::Vector3& up)
	: KGXCameraComponent(owner, glm::half_pi<float>(), 1.0f, 0.001f, 5000.0f, eye, target, up)
{
}

KGXCameraComponent::KGXCameraComponent(KGXSceneObject* owner, float fovY, float aspect, float nearZ, float farZ,
				const math::Vector3& eye, const math::Vector3& target, const math::Vector3& up)
	: KGXSceneObjectComponent(owner), mProjMatrix(), mViewMatrix(), mEye(eye), mTarget(target), mCamUp(up),
	mFov(fovY), mAspectRatio(aspect), mNearZ(nearZ), mFarZ(farZ)
{
	mProjMatrix = glm::perspectiveLH_ZO(mFov, mAspectRatio, mNearZ, mFarZ);

	lookAt(mEye, mTarget, mCamUp);
}

const math::Matrix4X4& KGXCameraComponent::getProjMatrix() const
{
	return mProjMatrix;
}

const math::Matrix4X4& KGXCameraComponent::getViewMatrix() const
{
	return mViewMatrix;
}

const math::Matrix4X4& KGXCameraComponent::getViewProjMatrix() const
{
	return mViewProjMatrix;
}

const math::Vector3& KGXCameraComponent::getEye() const
{
	return mEye;
}

const math::Vector3& KGXCameraComponent::getTarget() const
{
	return mTarget;
}

const math::Vector3& KGXCameraComponent::getUp() const
{
	return mCamUp;
}

float KGXCameraComponent::getFOV() const
{
	return mFov;
}

float KGXCameraComponent::getAspectRatio() const
{
	return mAspectRatio;
}

float KGXCameraComponent::getNearZ() const
{
	return mNearZ;
}

float KGXCameraComponent::getFarZ() const
{
	return mFarZ;
}

void KGXCameraComponent::lookAt(const math::Vector3& eye, const math::Vector3& target, const math::Vector3& up)
{
	mViewMatrix = glm::lookAtLH(eye, target, up);
	mViewProjMatrix = mProjMatrix * mViewMatrix;

	mEye = eye;
	mTarget = target;
	mCamUp = up;
}

void KGXCameraComponent::moveForward(float dist)
{
	translate(glm::normalize(mTarget - mEye) * dist);
}

void KGXCameraComponent::moveBackward(float dist)
{
	moveForward(-dist);
}

void KGXCameraComponent::moveLeft(float dist)
{
	moveRight(-dist);
}

void KGXCameraComponent::moveRight(float dist)
{
	const math::Vector3 dirVect = mTarget - mEye;
	const math::Vector3 rightVect = glm::normalize(glm::cross(dirVect, mCamUp));
	translate(rightVect * dist);
}

void KGXCameraComponent::translate(const math::Vector3& deltaPos)
{
	lookAt(mEye + deltaPos, mTarget + deltaPos, mCamUp);
}

void KGXCameraComponent::rotateUp(float degrees)
{
	const math::Vector3 dirVect = glm::normalize(mTarget - mEye);

	const math::Vector3 rotAxis = glm::cross(dirVect, mCamUp);
	const math::Matrix4X4 rotMat = glm::rotate(math::Matrix4X4(1.0f), glm::radians(degrees), rotAxis);

	const math::Vector4 newTarget = rotMat * math::Vector4(dirVect, 1.0f);
	mTarget = math::Vector3(newTarget.x, newTarget.y, newTarget.z) + mEye;

	lookAt(mEye, mTarget, mCamUp);
}

void KGXCameraComponent::rotateDown(float degrees)
{
	rotateUp(-degrees);
}

void KGXCameraComponent::rotateLeft(float degrees)
{
	const math::Vector3 dirVect = glm::normalize(mTarget - mEye);

	const math::Vector3 rightVect = glm::cross(dirVect, mCamUp);
	const math::Vector3 rotAxis = glm::normalize(glm::cross(rightVect, dirVect));
	const math::Matrix4X4 rotMat = glm::rotate(math::Matrix4X4(1.0f), glm::radians(degrees), rotAxis);

	const math::Vector4 newTarget = rotMat * math::Vector4(dirVect, 1.0f);
	mTarget = math::Vector3(newTarget.x, newTarget.y, newTarget.z) + mEye;

	lookAt(mEye, mTarget, mCamUp);
}

void KGXCameraComponent::rotateRight(float degrees)
{
	rotateLeft(-degrees);
}

void KGXCameraComponent::activateCamera()
{
	core::RenderCore::get()->getScenePtr()->setActiveCamera(this);
}

bool KGXCameraComponent::initializeImpl()
{
	if (!core::RenderCore::get()->getScenePtr()->hasActiveCamera())
	{
		activateCamera();
	}

	return true;
}
}
