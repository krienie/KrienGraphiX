
#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Texture.h"

namespace kgx
{
	/* Light information for passing to the shaders */
	struct Light
	{
		//TODO: add colored light support
		DirectX::XMFLOAT3 direction;
		float intensity;
	};

	struct LightData
	{
		DirectX::XMFLOAT4 ambientLight;
		std::vector<Light> lights;
	};

	struct Material
	{
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 specular;
		Texture::TextureID diffuseMap;
		Texture::TextureID specularMap;
		Texture::TextureID normalMap;
	};

	/* vertex- and index buffers */
	typedef UINT MeshBufferID;
	struct MeshBuffer
	{
		UINT vertexBufferStride;
		ID3D11Buffer *vertBuff, *indexBuff;
	};
}
