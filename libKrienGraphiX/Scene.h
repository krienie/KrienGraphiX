
#pragma once

#include <vector>

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
			// no copying allowed
			Scene( const Scene& );
			Scene& operator=( const Scene& );

			std::vector<RenderableObject*> m_renderObjects;

	};
}
