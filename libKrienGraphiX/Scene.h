
#pragma once

#include <DirectXMath.h>
#include <vector>

#include "Defines.h"

namespace kgx
{
	class Camera;
	class RenderableObject;

	class Scene
	{
		public:
			Scene();
			~Scene();

			void addDirectionalLight( const DirectX::XMFLOAT3 &direction, float intensity );

			/** Adds a RenderableObject to the scene. Takes ownership of the object */
			void claimRenderableObject( _In_ RenderableObject *obj );
			void render( _In_ Camera *renderCam );

		private:
			// no copying allowed
			Scene( const Scene& );
			Scene& operator=( const Scene& );

			//TODO: consider making this a float3
			DirectX::XMFLOAT4 m_ambientLight;

			std::vector<Light> m_lights;
			std::vector<RenderableObject*> m_renderObjects;
	};
}
