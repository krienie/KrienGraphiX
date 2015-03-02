
#pragma once

#include <vector>

//#include "RenderableObject.h"


namespace kgx
{
	class Camera;
	class RenderableObject;

	class Scene
	{
		public:
			Scene();
			~Scene();

			/** Adds a RenderableObject to the scene. Takes ownership of the object */
			void claimRenderableObject( _In_ RenderableObject *obj );
			void render( _In_ Camera *renderCam );

		private:
			std::vector<RenderableObject*> renderObjects;

	};
}
