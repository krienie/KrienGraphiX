
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "VertexInputLayout.h"

namespace kgx
{
	/* Light information for passing to the shaders */
	struct Light
	{
		Light( const DirectX::XMFLOAT3 &dir, float intens ) : direction( dir ), intensity( intens ) {}

		//TODO: add colored light support
		DirectX::XMFLOAT3 direction;
		float intensity;
	};

	/* vertex- and index buffers */
	struct MeshBuffer
	{
		MeshBuffer()
			: vertBuff(0), indexBuff(0), inputDescriptor() {}

		ID3D11Buffer *vertBuff, *indexBuff;
		VertexInputLayout inputDescriptor;
	};
}
