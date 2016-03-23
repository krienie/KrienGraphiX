
#pragma once

#include <DirectXMath.h>
#include <map>
#include <vector>

#include "Defines.h"

namespace kgx
{
	class Camera;
	class ShaderProgram;
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

			typedef std::map<CameraID, Camera*>::const_iterator const_cameraiterator;
			const_cameraiterator getCameraCBegin() const;
			const_cameraiterator getCameraCEnd() const;

			typedef std::vector<RenderableObject*>::const_iterator const_renobjectiterator;
			const_renobjectiterator getRenObjectCBegin() const;
			const_renobjectiterator getRenObjectCEnd() const;

			typedef std::vector<Light>::const_iterator const_lightiterator;
			const_lightiterator getLightCBegin() const;
			const_lightiterator getLightCEnd() const;

			Camera* getCamera( CameraID id ) const;
			Camera* getDefaultCamera() const;
			void setDefaultCamera( CameraID id );

			DirectX::XMFLOAT3 getAmbient() const;
			void setAmbient( const DirectX::XMFLOAT3 &color );
			void setAmbient( float r, float g, float b );
			void addDirectionalLight( const DirectX::XMFLOAT3 &direction, float intensity, const std::string &name = "" );

			/** Adds a RenderableObject to the scene. Takes ownership of the object */
			void claimRenderableObject( RenderableObject *obj );

			void render( Camera *renderCam, ShaderProgram *shaderProg );

		private:
			// no copying allowed
			Scene( const Scene& );
			Scene& operator=( const Scene& );

			DirectX::XMFLOAT4 m_ambientLight;

			CameraID m_nextCamID;
			Camera *m_defaultCamera;

			std::map<CameraID, Camera*> m_cameras;
			std::vector<Light> m_lights;
			std::vector<RenderableObject*> m_renderObjects;
	};
}
