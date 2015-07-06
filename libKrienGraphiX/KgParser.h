
#pragma once

#include <string>

#include "parser_defines.h"

namespace kgx
{
	class RenderableObject;
	class Shader;
	class VertexShader;
	class PixelShader;

	class KgParser
	{
		public:
			static RenderableObject* loadKGO( const std::string &kgoFile );

		private:
			KgParser();
			~KgParser();

			// no copying allowed
			KgParser( const KgParser& );
			KgParser& operator=( const KgParser& );

			static RenderableObject* renderableObjectFromParseData( std::vector<float> vertices, std::vector<UINT> &indices,
																	std::vector<VertexInputLayout::Type> &vertLayoutTypes,
																	std::vector<KgModelData> &models,
																	std::map<std::string, KgMatData> &materials );

			static void setShaderVariables( Material *material, ShaderBase *sh, const KgMatData::ShaderDef &shDef );
	};
}
