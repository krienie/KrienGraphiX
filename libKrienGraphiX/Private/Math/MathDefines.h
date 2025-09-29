
#pragma once

#include <DirectXMath.h>

namespace kgx::math
{
typedef DirectX::XMFLOAT2 Vector2;
typedef DirectX::XMFLOAT3 Vector3;
typedef DirectX::XMFLOAT4 Vector4;
typedef DirectX::XMFLOAT4X4 Matrix4X4;

static Matrix4X4 IdentityMatrix4X4;

void initMathDefines();
}

//TODO(KL): If we define the math defines as DirectX::XMVECTOR and DirectX::XMMATRIX these operator defines below are not needed...
[[nodiscard]]
inline kgx::math::Matrix4X4 operator*(const kgx::math::Matrix4X4& lhs, const kgx::math::Matrix4X4& rhs)
{
	using namespace DirectX;
	
	XMFLOAT4X4 storedMatrix;
	XMStoreFloat4x4(&storedMatrix, XMMatrixMultiply(XMLoadFloat4x4(&lhs), XMLoadFloat4x4(&rhs)));

	return storedMatrix;
}
