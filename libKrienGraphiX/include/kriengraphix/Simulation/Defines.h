
#pragma once

#include <vector>
#include <DirectXMath.h>

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
		std::string diffuseMap;
		std::string specularMap;
		std::string normalMap;
	};
}
