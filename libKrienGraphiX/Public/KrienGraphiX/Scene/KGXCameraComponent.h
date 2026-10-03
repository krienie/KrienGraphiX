
#pragma once

#include "KrienGraphiX/Math/MathDefines.h"

#include "KGXSceneObjectComponent.h"

namespace kgx
{
class KGXCameraComponent : public KGXSceneObjectComponent
{
public:
	KGXCameraComponent(KGXSceneObject *owner, const math::Vector3 &eye, const math::Vector3 &target, const math::Vector3 &up);
	KGXCameraComponent(KGXSceneObject *owner, float fovY, float aspect, float nearZ, float farZ,
			const math::Vector3 &eye = math::Vector3(0.0f, 0.0f, 0.0f),
			const math::Vector3 &target = math::Vector3(0.0f, 0.0f, -1.0f),
			const math::Vector3 &up = math::Vector3(0.0f, 1.0f, 0.0f));
	~KGXCameraComponent() override = default;

	[[nodiscard]] const math::Matrix4X4& getProjMatrix() const;
	[[nodiscard]] const math::Matrix4X4& getViewMatrix() const;
	[[nodiscard]] const math::Matrix4X4& getViewProjMatrix() const;
	[[nodiscard]] const math::Vector3& getEye() const;
	[[nodiscard]] const math::Vector3& getTarget() const;
	[[nodiscard]] const math::Vector3& getUp() const;
	[[nodiscard]] float getFOV() const;
	[[nodiscard]] float getAspectRatio() const;
	[[nodiscard]] float getNearZ() const;
	[[nodiscard]] float getFarZ() const;

	//TODO(KL): Al deze bewegings dingen kunnen in een MovementComponent worden gezet.
	void lookAt(const math::Vector3 &eye, const math::Vector3 &target, const math::Vector3 &up);
	void moveForward(float dist);
	void moveBackward(float dist);
	void moveLeft(float dist);
	void moveRight(float dist);
	void translate(const math::Vector3 &deltaPos);
	//TODO: add support for radians and quaternions
	void rotateUp(float degrees);
	void rotateDown(float degrees);
	void rotateLeft(float degrees);
	void rotateRight(float degrees);

	void activateCamera();

private:
	bool initializeImpl() override;

	math::Matrix4X4 mProjMatrix;
	math::Matrix4X4 mViewMatrix;
	math::Matrix4X4 mViewProjMatrix;

	math::Vector3 mEye;
	math::Vector3 mTarget;
	math::Vector3 mCamUp;

	float mFov;
	float mAspectRatio;
	float mNearZ;
	float mFarZ;
};
}
