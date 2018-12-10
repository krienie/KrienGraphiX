
#pragma once

#include <string>

namespace kgx
{
	class RenderWindow;
	class Scene;

	class KGSceneParser
	{
		public:
			static Scene* loadKGScene( const std::string &kgsceneFile, RenderWindow *renderWin = nullptr );

			KGSceneParser()  = delete;
			~KGSceneParser() = delete;

		private:
			static bool loadRenderObject( const std::string &objString, Scene *parentScene );
			static bool loadCamera( const std::string &camString, RenderWindow *renderWin, Scene *parentScene );
			static bool loadLight( const std::string &lightString, Scene *parentScene );
	};
}
