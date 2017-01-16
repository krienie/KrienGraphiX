
#pragma once

#include <DirectXMath.h>
#include <map>
#include <vector>

#include "Defines.h"
#include "RenderableObject.h"

namespace kgx
{
	class Camera;
	class ShaderProgram;

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

			typedef std::vector<RenderableObject>::const_iterator const_renobjectiterator;
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
			void addDirectionalLight( const DirectX::XMFLOAT3 &direction, float intensity );

			/** Adds a RenderableObject to the scene */
			void addRenderableObject( const RenderableObject &obj );

			void render( Camera *renderCam, ID3D11RenderTargetView *rtv, ID3D11DepthStencilView *dsv );

		private:
			// no copying allowed
			Scene( const Scene& );
			Scene& operator=( const Scene& );

			UINT createCommandKey( const RenderableObject &obj ) const;

			ID3D11DeviceContext *m_dxDeferredDevCont;

			LightData m_lightData;

			CameraID m_nextCamID;
			Camera *m_defaultCamera;

			std::map<CameraID, Camera*> m_cameras;
			std::vector<RenderableObject> m_renderObjects;
	};
}
