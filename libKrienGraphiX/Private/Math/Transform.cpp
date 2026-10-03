
#include "KrienGraphiX/Math/Transform.h"

namespace
{
kgx::math::Matrix4X4 makeTransformMatrix(const kgx::math::Quaternion& rotQuat, const kgx::math::Vector3& translation, const kgx::math::Vector3& scale)
{
	glm::mat4 transform = glm::mat4_cast(rotQuat);

	transform[0] *= scale.x;
	transform[1] *= scale.y;
	transform[2] *= scale.z;
	transform[3] = glm::vec4(translation, 1.0f);

	return transform;
}
}

namespace kgx::math
{
Transform::Transform()
	: mRotation(1.0f, 0, 0, 0), mTranslation(0, 0, 0), mScale(1, 1, 1)
{
}

Matrix4X4 Transform::getMatrix() const
{
	return makeTransformMatrix(mRotation, mTranslation, mScale);
}

Matrix4X4 Transform::getInverseTransposeMatrix() const
{
	const Matrix4X4 transMat = makeTransformMatrix(mRotation, mTranslation, mScale);
	return glm::transpose(glm::inverse(transMat));
}

void Transform::setTranslation(const Vector3& translation)
{
	mTranslation = translation;
}

void Transform::setRotation(const Quaternion& rotation)
{
	mRotation = rotation;
}

void Transform::setScale(const Vector3& scale)
{
	mScale = scale;
}

Vector3 Transform::getTranslation() const
{
	return mTranslation;
}

Quaternion Transform::getRotation() const
{
	return mRotation;
}

Vector3 Transform::getScale() const
{
	return mScale;
}

float Transform::getPitch() const
{
	return pitch(mRotation);
}

float Transform::getYaw() const
{
	return yaw(mRotation);
}

float Transform::getRoll() const
{
	return roll(mRotation);
}
}
