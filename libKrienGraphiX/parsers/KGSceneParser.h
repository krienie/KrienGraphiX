
#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>

namespace kgx
{
	class RenderableObject;
	class RenderWindow;
	class Scene;

	class KGSceneParser
	{
		public:
			static Scene* loadKGScene( const std::string &kgsceneFile, RenderWindow *renderWin = nullptr );

		private:
			static RenderableObject* loadRenderObject( const std::string &objString );
			static bool loadCamera( const std::string &camString, RenderWindow *renderWin, Scene* parentScene );
			static bool loadLight( const std::string &lightString, Scene *parentScene );

			KGSceneParser();
			~KGSceneParser();

			// no copying allowed
			KGSceneParser( const KGSceneParser& );
			KGSceneParser& operator=(const KGSceneParser&);
	};
}
