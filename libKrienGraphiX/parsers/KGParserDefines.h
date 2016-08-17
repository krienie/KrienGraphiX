
#pragma once

#include <vector>

#include "../ShaderProgram.h"
#include "../VertexInputLayout.h"

typedef unsigned int UINT;

namespace kgx
{
	struct KgModelData
	{
		KgModelData( const std::string &mName = "", const std::string &usemtl = "", int start = 0, int count = 0 )
			: modelName(mName), matName(usemtl), startIndex(start), indexCount(count) {}

		std::string modelName;
		int startIndex;
		int indexCount;
		std::string matName;
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


	struct KgoData
	{
		KgoData() : inputLayout(), vertices(), indices(), models(), mats() {}

		std::vector<VertexInputLayout::Type> inputLayout;
		std::vector<float> vertices;
		std::vector<UINT> indices;
		std::vector<KgModelData> models;
		std::vector<KgMatData> mats;
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
