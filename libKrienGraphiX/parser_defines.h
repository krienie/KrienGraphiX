
#pragma once

#include <vector>

#include "Material.h"
#include "VertexInputLayout.h"

typedef unsigned int UINT;

namespace kgx
{
	struct KgModelData
	{
		KgModelData( const std::string &mName = "", const std::string &usemtl = "", int start = 0, int count = 0 )
			: modelName(mName), matName(usemtl), startIndex(start), indexCount(count) { }

		std::string modelName;
		int startIndex;
		int indexCount;
		std::string matName;
	};


	struct KgMatData
	{
		struct ShaderVar
		{
			ShaderVar() : name(), type(), defaultValue(), autoBindType(Material::ShaderAutoBindType::NoAutoBind) {}

			Material::ShaderAutoBindType autoBindType;
			std::string type;
			std::string name;
			std::string defaultValue;
		};

		struct ShaderDef
		{
			ShaderDef() : name(), variables() {}

			std::string name;
			std::vector<ShaderVar> variables;
		};

		KgMatData() : name(), vertexShader(), pixelShader() {}

		std::string name;
		//TODO: add support for geometry, hull and domain shaders
		ShaderDef vertexShader;
		ShaderDef pixelShader;

		//TODO: add support for textures + automatic texture binding
		//std::vector<std::string> textures;
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
}