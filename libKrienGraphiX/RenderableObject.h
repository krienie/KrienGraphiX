
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

		//TODO: place material data directly in this struct. It is quite pointless to let this live in a separate struct
		Material material;
		ShaderProgram::ShaderProgramID shaderProgram;
		
		float xPos;
		float yPos;
		float zPos;
		float xScale;
		float yScale;
		float zScale;
	};

	DirectX::XMFLOAT4X4 getModelMatrix( RenderableObject &obj );
	DirectX::XMFLOAT4X4 getNormalMatrix( RenderableObject &obj );
}
