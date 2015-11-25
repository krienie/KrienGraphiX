
#pragma once

#include <DirectXMath.h>
#include <map>
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

			typedef UINT CameraID;
			CameraID createCamera( const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up );
			CameraID createCamera( float fovY, float aspect, float nearZ, float farZ,
							   const DirectX::XMFLOAT3 &eye = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f ),
							   const DirectX::XMFLOAT3 &target = DirectX::XMFLOAT3( 0.0f, 0.0f, -1.0f ),
							   const DirectX::XMFLOAT3 &up = DirectX::XMFLOAT3( 0.0f, 1.0f, 0.0f ) );

			typedef std::map<CameraID, Camera*>::iterator CameraIterator;
			CameraIterator getCameraBegin();
			CameraIterator getCameraEnd();

			Camera* getCamera( CameraID id ) const;
			Camera* getDefaultCamera() const;
			void setDefaultCamera( CameraID id );

			void setAmbient( const DirectX::XMFLOAT3 &color );
			void setAmbient( float r, float g, float b );
			void addDirectionalLight( const DirectX::XMFLOAT3 &direction, float intensity );

			/** Adds a RenderableObject to the scene. Takes ownership of the object */
			void claimRenderableObject( _In_ RenderableObject *obj );

			//TODO: add functionality to find RO in the scene

			void render( _In_ Camera *renderCam );
			void renderDefaultCam();

		private:
			// no copying allowed
			Scene( const Scene& );
			Scene& operator=( const Scene& );

			//TODO: consider making this a float3
			DirectX::XMFLOAT4 m_ambientLight;

			CameraID m_nextCamID;
			Camera *m_defaultCamera;

			std::map<CameraID, Camera*> m_cameras;
			std::vector<Light> m_lights;
			std::vector<RenderableObject*> m_renderObjects;
	};
}
