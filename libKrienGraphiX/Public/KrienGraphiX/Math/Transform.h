
#pragma once

#include "MathDefines.h"

namespace kgx::math
{
class Transform
{
public:
	Transform();
	~Transform() = default;

	void setTranslation(float xPos, float yPos, float zPos);
	void setRotation(float pitch, float yaw, float roll);
	void setScale(float xScale, float yScale, float zScale);

	[[nodiscard]]
	Matrix4X4 getMatrix() const;

	[[nodiscard]]
	Matrix4X4 getInverseTransposeMatrix() const;

	float getPitch() const;
	float getYaw() const;
	float getRoll() const;

private:
	Quaternion mRotation;
	Vector3 mTranslation;
	Vector3 mScale;

	float mPitch;
	float mYaw;
	float mRoll;
};
}
