
#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>

#include "KGParserDefines.h"

namespace kgx
{
	class RenderableObject;
	class RenderCore;
	class RenderWindow;
	class Scene;
	class ShaderBase;
	class ShaderProgram;

	class KGSceneParser
	{
		public:
			static Scene* loadKGScene( const std::string &kgsceneFile, RenderWindow *renderWin = nullptr );

		private:
			static RenderableObject* loadRenderObject( const std::string &objString );
			static bool loadCamera( const std::string &camString, RenderWindow *renderWin, Scene* parentScene );
			static bool loadLight( const std::string &lightString, Scene *parentScene );
			static bool loadRenderPass( const std::string &passString, RenderCore *renderCore );

			static void setShaderVariables( ShaderProgram *ShaderProgram, ShaderBase *sh, const KgShaderProgramData::ShaderDef &shDef );

			KGSceneParser();
			~KGSceneParser();

			// no copying allowed
			KGSceneParser( const KGSceneParser& );
			KGSceneParser& operator=(const KGSceneParser&);
	};
}
