
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
	: mRotation(1.0f, 0, 0, 0), mTranslation(0, 0, 0), mScale(1, 1, 1),
	mPitch(0), mYaw(0), mRoll(0)
{
}

void Transform::setTranslation(float xPos, float yPos, float zPos)
{
	mTranslation.x = xPos;
	mTranslation.y = yPos;
	mTranslation.z = zPos;
}

void Transform::setRotation(float pitch, float yaw, float roll)
{
	mRotation = Quaternion(Vector3(pitch, yaw, roll));

	mPitch = pitch;
	mYaw = yaw;
	mRoll = roll;
}

void Transform::setScale(float xScale, float yScale, float zScale)
{
	mScale.x = xScale;
	mScale.y = yScale;
	mScale.z = zScale;
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
