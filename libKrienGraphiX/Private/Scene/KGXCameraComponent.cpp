
#include "KrienGraphiX/Scene/KGXCameraComponent.h"

#include "KrienGraphiX/Scene/KGXSceneObject.h"
#include "Private/Core/RenderCore.h"

namespace kgx
{
KGXCameraComponent::KGXCameraComponent(KGXSceneObject *owner, const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &m_target, const DirectX::XMFLOAT3 &up)
	: KGXCameraComponent(owner, DirectX::XM_PIDIV2, 1.0f, 0.001f, 5000.0f, eye, m_target, up)
{
}

KGXCameraComponent::KGXCameraComponent(KGXSceneObject *owner, float fovY, float aspect, float m_nearZ, float m_farZ,
				const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up)
	: KGXSceneObjectComponent(owner), mProjMatrix(), mViewMatrix(), mEye(eye), mTarget(target), mCamUp(up),
	mFov(fovY), mAspectRatio(aspect), mNearZ(m_nearZ), mFarZ(m_farZ)
{
	DirectX::XMMATRIX localPersp = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, m_nearZ, m_farZ);
	DirectX::XMStoreFloat4x4(&mProjMatrix, localPersp);

	lookAt(mEye, mTarget, mCamUp);
}

const DirectX::XMFLOAT4X4& KGXCameraComponent::getProjMatrix() const
{
	return mProjMatrix;
}

const DirectX::XMFLOAT4X4& KGXCameraComponent::getViewMatrix() const
{
	return mViewMatrix;
}

const DirectX::XMFLOAT4X4& KGXCameraComponent::getViewProjMatrix() const
{
	return mViewProjMatrix;
}

const DirectX::XMFLOAT3& KGXCameraComponent::getEye() const
{
	return mEye;
}

const DirectX::XMFLOAT3& KGXCameraComponent::getTarget() const
{
	return mTarget;
}

const DirectX::XMFLOAT3& KGXCameraComponent::getUp() const
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

void KGXCameraComponent::lookAt(const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up)
{
	// create CameraComponent view matrix
	const DirectX::XMVECTORF32 xmEye = { eye.x, eye.y, eye.z, 0.0f };
	const DirectX::XMVECTORF32 xmTarget = { target.x, target.y, target.z, 0.0f };
	const DirectX::XMVECTORF32 xmUp = { up.x, up.y, up.z, 0.0f };
	DirectX::XMMATRIX localView = DirectX::XMMatrixLookAtLH(xmEye, xmTarget, xmUp);
	DirectX::XMStoreFloat4x4(&mViewMatrix, localView);

	DirectX::XMMATRIX localProj = DirectX::XMLoadFloat4x4(&mProjMatrix);
	DirectX::XMMATRIX localViewProj = DirectX::XMMatrixMultiply(localView, localProj);
	DirectX::XMStoreFloat4x4(&mViewProjMatrix, localViewProj);

	mEye = eye;
	mTarget = target;
	mCamUp = up;
}

void KGXCameraComponent::moveForward(float dist)
{
	const DirectX::XMVECTOR eyeVect = DirectX::XMLoadFloat3(&mEye);
	const DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3(&mTarget);
	DirectX::XMVECTOR dirVect = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(targetVect, eyeVect));
	dirVect = DirectX::XMVectorScale(dirVect, dist);

	DirectX::XMFLOAT3 dirFloat3;
	DirectX::XMStoreFloat3(&dirFloat3, dirVect);

	translate(dirFloat3);
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
	const DirectX::XMVECTOR eyeVect = DirectX::XMLoadFloat3(&mEye);
	const DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3(&mTarget);
	const DirectX::XMVECTOR dirVect = DirectX::XMVectorSubtract(targetVect, eyeVect);

	const DirectX::XMVECTOR upVect = DirectX::XMLoadFloat3(&mCamUp);
	DirectX::XMVECTOR rightVect = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(dirVect, upVect));
	rightVect = DirectX::XMVectorScale(rightVect, dist);

	DirectX::XMFLOAT3 rightFloat3;
	DirectX::XMStoreFloat3(&rightFloat3, rightVect);

	translate(rightFloat3);
}

void KGXCameraComponent::translate(const DirectX::XMFLOAT3 &deltaPos)
{

	const DirectX::XMFLOAT3 newPos = DirectX::XMFLOAT3(mEye.x + deltaPos.x,
													   mEye.y + deltaPos.y,
													   mEye.z + deltaPos.z);
	const DirectX::XMFLOAT3 newTarget = DirectX::XMFLOAT3(mTarget.x + deltaPos.x,
														  mTarget.y + deltaPos.y,
														  mTarget.z + deltaPos.z);
	lookAt(newPos, newTarget, mCamUp);
}

void KGXCameraComponent::rotateUp(float degrees)
{
	const DirectX::XMVECTOR upVect = DirectX::XMLoadFloat3(&mCamUp);
	const DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3(&mTarget);
	const DirectX::XMVECTOR eyeVect = DirectX::XMLoadFloat3(&mEye);
	const DirectX::XMVECTOR dirVect = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(targetVect, eyeVect));

	const DirectX::XMVECTOR rotAxis = DirectX::XMVector3Cross(dirVect, upVect);
	const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(rotAxis, DirectX::XMConvertToRadians(degrees));

	DirectX::XMVECTOR newTarget = DirectX::XMVectorAdd(DirectX::XMVector3Transform(dirVect, rotMat), eyeVect);
	DirectX::XMStoreFloat3(&mTarget, newTarget);

	lookAt(mEye, mTarget, mCamUp);
}

void KGXCameraComponent::rotateDown(float degrees)
{
	rotateUp(-degrees);
}

void KGXCameraComponent::rotateLeft(float degrees)
{
	const DirectX::XMVECTOR upVect = DirectX::XMLoadFloat3(&mCamUp);
	const DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3(&mTarget);
	const DirectX::XMVECTOR eyeVect = DirectX::XMLoadFloat3(&mEye);
	const DirectX::XMVECTOR dirVect = DirectX::XMVectorSubtract(targetVect, eyeVect);

	//TODO(KL): fix assertion error when dirVect == (0.0, -1.0, 0.0) and upVect == (0.0, 1.0, 0.0)
	const DirectX::XMVECTOR rightVect = DirectX::XMVector3Cross(dirVect, upVect);
	const DirectX::XMVECTOR rotAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(rightVect, dirVect));
	const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(rotAxis, DirectX::XMConvertToRadians(degrees));

	DirectX::XMVECTOR newTarget = DirectX::XMVectorAdd(DirectX::XMVector3Transform(dirVect, rotMat), eyeVect);
	DirectX::XMStoreFloat3(&mTarget, newTarget);

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
