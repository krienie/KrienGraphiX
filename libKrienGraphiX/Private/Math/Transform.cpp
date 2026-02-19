
#include "Transform.h"

#include <DirectXMath.h>

namespace
{
DirectX::XMMATRIX makeTransformMatrix(DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 translation, DirectX::XMFLOAT3 scale)
{
	//TODO(KL): Hide this directX stuff behind an abstraction layer
	const DirectX::XMVECTOR loadedQuat = DirectX::XMLoadFloat4(&rotation);
	const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationQuaternion(loadedQuat);

	const DirectX::XMVECTOR loadedTranslation = DirectX::XMLoadFloat3(&translation);
	const DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslationFromVector(loadedTranslation);

	const DirectX::XMVECTOR loadedScale = DirectX::XMLoadFloat3(&scale);
	const DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScalingFromVector(loadedScale);
	return XMMatrixMultiply(rotMat, XMMatrixMultiply(scaleMat, transMat));
}
}

namespace kgx::math
{
using namespace DirectX;

Transform::Transform()
	: mTranslation(0, 0, 0), mScale(1, 1, 1),
	mPitch(0), mYaw(0), mRoll(0)
{
	const XMVECTOR identityQuat = XMQuaternionIdentity();
	XMStoreFloat4(&mRotation, identityQuat);
}

void Transform::setTranslation(float xPos, float yPos, float zPos)
{
	mTranslation = XMFLOAT3(xPos, yPos, zPos);
}

void Transform::setRotation(float pitch, float yaw, float roll)
{
	XMVECTOR newQuat = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
	XMStoreFloat4(&mRotation, newQuat);

	mPitch = pitch;
	mYaw = yaw;
	mRoll = roll;
}

void Transform::setScale(float xScale, float yScale, float zScale)
{
	mScale = XMFLOAT3(xScale, yScale, zScale);
}

XMFLOAT4X4 Transform::getMatrix() const
{
	const XMMATRIX transMat = makeTransformMatrix(mRotation, mTranslation, mScale);

	XMFLOAT4X4 transformMatrix;
	XMStoreFloat4x4(&transformMatrix, transMat);

	return transformMatrix;
}

XMFLOAT4X4 Transform::getInverseTransposeMatrix() const
{
	const XMMATRIX transMat = makeTransformMatrix(mRotation, mTranslation, mScale);
	XMMATRIX normalMat = XMMatrixInverse(nullptr, transMat);
	normalMat = XMMatrixTranspose(normalMat);

	XMFLOAT4X4 inverseTransposeMatrix;
	XMStoreFloat4x4(&inverseTransposeMatrix, normalMat);

	return inverseTransposeMatrix;
}

float Transform::getPitch() const
{
	return mPitch;
}

float Transform::getYaw() const
{
	return mYaw;
}

float Transform::getRoll() const
{
	return mRoll;
}
}
