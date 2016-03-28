
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


	struct KgShaderProgramData
	{
		struct ShaderVar
		{
			ShaderVar() : name(), type(), defaultValue(), autoBindType(ShaderProgram::ShaderAutoBindType::NoAutoBind) {}

			ShaderProgram::ShaderAutoBindType autoBindType;
			std::string type;
			std::string name;
			std::string defaultValue;
		};

		struct ShaderDef
		{
			ShaderDef() : filename(), variables() {}

			std::string filename;
			std::vector<ShaderVar> variables;
			std::vector<std::string> textures;
		};

		KgShaderProgramData() : name(), vertexShader(), pixelShader() {}

		std::string name;
		//TODO: add support for geometry, hull and domain shaders
		ShaderDef vertexShader;
		ShaderDef pixelShader;
	};

	struct KgMatData
	{
		KgMatData() : name(), diffuse(0.0f, 0.0f, 0.0f, 1.0f), specular(0.0f, 0.0f, 0.0f, 1.0f) {}

		std::string       name;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 specular;
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
