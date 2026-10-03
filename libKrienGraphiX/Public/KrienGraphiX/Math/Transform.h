
#pragma once

#include "MathDefines.h"

namespace kgx::math
{
class Transform
{
public:
	Transform();
	~Transform() = default;

	[[nodiscard]]
	Matrix4X4 getMatrix() const;

	[[nodiscard]]
	Matrix4X4 getInverseTransposeMatrix() const;

	void setTranslation(const Vector3& translation);
	void setRotation(const Quaternion& rotation);
	void setScale(const Vector3& scale);
	[[nodiscard]] Vector3 getTranslation() const;
	[[nodiscard]] Quaternion getRotation() const;
	[[nodiscard]] Vector3 getScale() const;

	[[nodiscard]] float getPitch() const;
	[[nodiscard]] float getYaw() const;
	[[nodiscard]] float getRoll() const;

private:
	Quaternion mRotation;
	Vector3 mTranslation;
	Vector3 mScale;
};
}
