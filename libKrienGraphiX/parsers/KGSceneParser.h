
#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>

#include "KGParserDefines.h"

namespace kgx
{
	class RenderWindow;
	class Scene;
	class ShaderBase;
	class ShaderProgram;

	class KGSceneParser
	{
		public:
			static Scene* loadKGScene( const std::string &kgsceneFile, RenderWindow *renderWin = nullptr );

		private:
			static bool loadRenderObject( const std::string &objString, Scene *parentScene );
			static bool loadCamera( const std::string &camString, RenderWindow *renderWin, Scene *parentScene );
			static bool loadLight( const std::string &lightString, Scene *parentScene );

			KGSceneParser();
			~KGSceneParser();

			// no copying allowed
			KGSceneParser( const KGSceneParser& );
			KGSceneParser& operator=(const KGSceneParser&);
	};
}
