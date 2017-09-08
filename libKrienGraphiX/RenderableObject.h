
#pragma once

#include <d3d11.h>
#include <string>
#include <vector>

#include "Defines.h"
#include "ShaderProgram.h"


namespace kgx
{
	struct RenderableObject
	{
		std::string name;

		D3D11_PRIMITIVE_TOPOLOGY topology;
		MeshBufferID meshBuffer;
		
		UINT indexCount;
		UINT startIndex;
		UINT baseVertex;

		Material material;
		
		float xPos;
		float yPos;
		float zPos;
		float xScale;
		float yScale;
		float zScale;
	};

	DirectX::XMFLOAT4X4 getModelMatrix( const RenderableObject &obj );
	DirectX::XMFLOAT4X4 getNormalMatrix( const RenderableObject &obj );
}
