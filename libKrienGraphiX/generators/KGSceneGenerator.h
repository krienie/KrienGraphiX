
#pragma once

#include <string>

#include "../Defines.h"

namespace kgx
{
	class Camera;
	class Scene;
	class RenderableObject;

	class KGSceneGenerator
	{
		public:
			static void generate( Scene *inputScene, const std::string &roSavePath, std::string &output );

		private:
			static bool generateRenderableObject( RenderableObject *ro, std::string &output );
			static bool generateCamera( Camera *cam, std::string &output );
			static bool generateAmbientLight( const DirectX::XMFLOAT3 &ambColor, std::string &output );
			static bool generateLight( const Light &li, std::string &output );

			KGSceneGenerator();
			//~KGSceneGenerator();

			// no copying allowed
			KGSceneGenerator( const KGSceneGenerator& );
			KGSceneGenerator& operator=(const KGSceneGenerator&);
	};
}
