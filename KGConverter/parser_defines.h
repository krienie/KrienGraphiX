
#pragma once

#include <vector>

#include "Material.h"
#include "VertexInputLayout.h"

typedef unsigned int UINT;

namespace kgx
{
	//TODO: Rename ModelData and MatData to something like KgModelData and KgMatData
	struct ModelData
	{
		ModelData( const std::string &mName, const std::string &usemtl, int start, int count )
			: modelName(mName), matName(usemtl), startIndex(start), indexCount(count) { }

		std::string modelName;
		int startIndex;
		int indexCount;
		std::string matName;
	};


	struct MatData
	{
		struct ShaderVar
		{
			ShaderVar() : name(), type(), defaultValue(), autoBindType(Material::ShaderAutoBindType::NoAutoBind) {}

			std::string name;
			std::string type;
			std::string defaultValue;
			Material::ShaderAutoBindType autoBindType;
		};

		struct ShaderDef
		{
			ShaderDef() : name(), variables() {}

			std::string name;
			std::vector<ShaderVar> variables;
		};

		MatData() : name(), vertexShader(), pixelShader() {}

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
		std::vector<ModelData> models;
		std::vector<MatData> mats;
	};

	/**
	* Benodigdheden voor het parsen (output van de parser)
	***** KGOBJ file (KrienGraphiX Object file => 3D model(s) + material(s)) *****
	* Voor het bouwen van MeshBuffer: const std::vector<float> &vertices, const std::vector<UINT> &indices, const VertexInputLayout &inputDescriptor
	* Voor Material:
	*		Path to Vertex Shader
	*		Path to Pixel Shader
	*		Path to evt. textures
	*		evt. AutoShaderVars + evt. default waarden voor buffers
	*
	***** KGSCENE file (KrienGraphiX Scene file => Scene with scene nodes (RenderableObjects, Camera's, Lights, etc.) *****

	*/
}
