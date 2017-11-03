
#pragma once

#include <string>
#include <vector>

#include <DirectXMath.h>

#include "../VertexInputLayout.h"

typedef unsigned int UINT;

namespace kgx
{
	struct KgModelData
	{
		KgModelData( const std::string &mName = "", const std::string &usemtl = "", int start = 0, int count = 0 )
			: modelName(mName), startIndex(start), indexCount(count), materialName(usemtl) {}

		std::string modelName;
		int startIndex;
		int indexCount;
		std::string materialName;
	};

	struct KgMatData
	{
		std::string       name;
		DirectX::XMFLOAT4 diffuse;
		std::string       diffuseMap;
		DirectX::XMFLOAT4 specular;
		std::string       specularMap;
		std::string       normalMap;
	};

	struct KgShaderProgData
	{
		std::string programName;
		std::string vertexShader;
		std::string hullShader;
		std::string domainShader;
		std::string geometryShader;
		std::string pixelShader;
		std::vector<VertexInputLayout::Type> vertLayoutTypes;
	};


	struct KgoData
	{
		KgoData() : inputLayout(), vertices(), indices(), models(), mats() {}

		std::vector<VertexInputLayout::Type> inputLayout;
		std::vector<float>       vertices;
		std::vector<UINT>        indices;
		std::vector<KgModelData> models;
		std::vector<KgMatData>   mats;
	};

	// the streaming operator for VertexInputLayout::Type
	inline std::ostream& operator<< (std::ostream& os, const VertexInputLayout::Type &type)
	{
		switch ( type )
		{
			case VertexInputLayout::Position:
				os << "Position";
				break;
			case VertexInputLayout::TextureCoordinate:
				os << "TextureCoordinate";
				break;
			case VertexInputLayout::Normal:
				os << "Normal";
				break;
			case VertexInputLayout::Tangent:
				os << "Tangent";
				break;
			default:
				break;
		}

		return os;
	}
}
