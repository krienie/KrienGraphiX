
#include <iostream>

#include "RenderableObject.h"

namespace kgx
{
	DirectX::XMFLOAT4X4 getModelMatrix( const RenderableObject &obj )
	{
		//TODO: maybe cache this, if necessary?

		DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation( obj.xPos, obj.yPos, obj.zPos );
		DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling( obj.xScale, obj.yScale, obj.zScale );
		DirectX::XMMATRIX modelMat = DirectX::XMMatrixMultiply( scaleMat, transMat );

		DirectX::XMFLOAT4X4 result;
		DirectX::XMStoreFloat4x4( &result, modelMat );

		return result;
	}

	DirectX::XMFLOAT4X4 getNormalMatrix( const RenderableObject &obj )
	{
		//TODO: maybe cache this, if necessary?

		// the normal matrix is the inverse-transpose of the model matrix
		DirectX::XMMATRIX modelMat  = DirectX::XMLoadFloat4x4( &getModelMatrix(obj) );
		DirectX::XMMATRIX normalMat = DirectX::XMMatrixInverse( nullptr, modelMat );
		normalMat = DirectX::XMMatrixTranspose( normalMat );

		DirectX::XMFLOAT4X4 result;
		DirectX::XMStoreFloat4x4( &result, normalMat );

		return result;
	}
}
