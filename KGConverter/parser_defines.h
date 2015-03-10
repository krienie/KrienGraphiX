
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#include <Windows.h>
#include <vector>

#include "Material.h"
#include "VertexInputLayout.h"

namespace kgx
{
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
		//TODO: add support for non-auto update variables
		struct ShaderVar
		{
			ShaderVar() : name(), type(), defaultValue(), autoBindType(Material::ShaderAutoBindType::NoAutoBind) {}

			std::string name;
			std::string type;
			std::string defaultValue;
			Material::ShaderAutoBindType autoBindType;
		};

		MatData() : name(), vertexShader(), pixelShader(), vertexAutoVars(), pixelAutoVars() {}

		std::string name;
		//TODO: add support for geometry, hull and domain shaders
		std::string vertexShader;
		std::string pixelShader;
		std::vector<ShaderVar> vertexAutoVars;
		std::vector<ShaderVar> pixelAutoVars;

		//TODO: add support for textures + automatic texture binding
		//std::vector<std::string> textures;
	};


	struct KgmData
	{
		KgmData() : vertices(), indices(), inputLayout(), models(), mats() {}

		std::vector<float> vertices;
		std::vector<UINT> indices;
		std::vector<VertexInputLayout::Type> inputLayout;
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
