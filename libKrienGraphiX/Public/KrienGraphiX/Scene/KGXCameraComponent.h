
#pragma once

#include <DirectXMath.h>

#include "KGXSceneObjectComponent.h"

namespace kgx
{
class KGXCameraComponent : public KGXSceneObjectComponent
{
public:
	KGXCameraComponent(KGXSceneObject *owner, const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up);
	KGXCameraComponent(KGXSceneObject *owner, float fovY, float aspect, float nearZ, float farZ,
			const DirectX::XMFLOAT3 &eye = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			const DirectX::XMFLOAT3 &target = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),
			const DirectX::XMFLOAT3 &up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	~KGXCameraComponent() override = default;

	const DirectX::XMFLOAT4X4& getProjMatrix() const;
	const DirectX::XMFLOAT4X4& getViewMatrix() const;
	const DirectX::XMFLOAT4X4& getViewProjMatrix() const;
	const DirectX::XMFLOAT3& getEye() const;
	const DirectX::XMFLOAT3& getTarget() const;
	const DirectX::XMFLOAT3& getUp() const;
	float getFOV() const;
	float getAspectRatio() const;
	float getNearZ() const;
	float getFarZ() const;

	//TODO(KL): Al deze bewegings dingen kunnen in een MovementComponent worden gezet.
	void lookAt(const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up);
	void moveForward(float dist);
	void moveBackward(float dist);
	void moveLeft(float dist);
	void moveRight(float dist);
	void translate(const DirectX::XMFLOAT3 &deltaPos);
	//TODO: add support for radians and quaternions
	void rotateUp(float degrees);
	void rotateDown(float degrees);
	void rotateLeft(float degrees);
	void rotateRight(float degrees);

	void activateCamera();

private:
	bool initializeImpl() override;

	DirectX::XMFLOAT4X4 mProjMatrix;
	DirectX::XMFLOAT4X4 mViewMatrix;
	DirectX::XMFLOAT4X4 mViewProjMatrix;

	DirectX::XMFLOAT3 mEye;
	DirectX::XMFLOAT3 mTarget;
	DirectX::XMFLOAT3 mCamUp;

	float mFov;
	float mAspectRatio;
	float mNearZ;
	float mFarZ;
};
}
