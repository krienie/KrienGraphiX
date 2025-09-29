
#include "MathDefines.h"

namespace kgx::math
{
void initMathDefines()
{
	DirectX::XMMATRIX identityMatrix = DirectX::XMMatrixIdentity();

	DirectX::XMStoreFloat4x4(&IdentityMatrix4X4, identityMatrix);
}
}
