
#include <iostream>
#include <d3d11.h>

#include "Camera.h"
#include "ResourceManager.h"
#include "renderpasses/RenderPass.h"

//TODO: temporary
#include "KGXCore.h"

#include "Scene.h"

namespace kgx
{
	Scene::Scene()
		: m_dxDeferredDevCont(nullptr), m_lightData(), m_nextCamID(0u),
			m_defaultCamera(nullptr), shaderProgPass1(-1), shaderProgPass2(-1),
			m_cameras(), m_renderObjects()
	{
		m_lightData.ambientLight = DirectX::XMFLOAT4( 0.25f, 0.25f, 0.25f, 1.0f );

		//TODO: temporary placed here. Need to find a better place for this
		
		// render pass 1
		ShaderProgram *prog = ResourceManager::getInst()->getShaderProgram("ForwardPass");
		shaderProgPass1 = prog->getID();

		// render pass 2
		/*prog = ResourceManager::getInst()->createShaderProgram();
		shaderProgPass2 = prog->getID();

		VertexInputLayout vertInputLayoutPass2;
		vertInputLayoutPass2.addInputType( VertexInputLayout::Position );
		prog->createVertexShader( "DeferredCompositionVS.cso", vertInputLayoutPass2 );
		prog->createPixelShader( "DeferredCompositionPS.cso" );*/
	}

	Scene::~Scene()
	{
		if ( m_dxDeferredDevCont )
			m_dxDeferredDevCont->Release();

		std::map<CameraID, Camera*>::iterator camIt;
		for ( camIt = m_cameras.begin(); camIt != m_cameras.end(); ++camIt )
			delete camIt->second;
	}


	Scene::CameraID Scene::createCamera( const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up )
	{
		return createCamera( DirectX::XM_PIDIV4, 1.0f, 0.001f, 500.0f, eye, target, up );
	}
	Scene::CameraID Scene::createCamera( float fovY, float aspect, float nearZ, float farZ,
					   const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up )
	{
		Camera *newCam = new Camera( this, fovY, aspect, nearZ, farZ, eye, target, up );
		m_cameras.insert( std::pair<CameraID, Camera*>(m_nextCamID, newCam) );
		++m_nextCamID;

		if ( !m_defaultCamera )
			m_defaultCamera = newCam;

		return m_nextCamID - 1u;
	}


	Scene::const_cameraiterator Scene::getCameraCBegin() const
	{
		return m_cameras.cbegin();
	}
	Scene::const_cameraiterator Scene::getCameraCEnd() const
	{
		return m_cameras.cend();
	}
	
	Scene::const_renobjectiterator Scene::getRenObjectCBegin() const
	{
		return m_renderObjects.cbegin();
	}
	Scene::const_renobjectiterator Scene::getRenObjectCEnd() const
	{
		return m_renderObjects.cend();
	}

	Scene::const_lightiterator Scene::getLightCBegin() const
	{
		return m_lightData.lights.cbegin();
	}
	Scene::const_lightiterator Scene::getLightCEnd() const
	{
		return m_lightData.lights.cend();
	}


	Camera* Scene::getCamera( CameraID id ) const
	{
		std::map<CameraID, Camera*>::const_iterator it = m_cameras.find( id );
		if ( it != m_cameras.end() )
			return it->second;

		return nullptr;
	}
	Camera* Scene::getDefaultCamera() const
	{
		return m_defaultCamera;
	}
	void Scene::setDefaultCamera( CameraID id )
	{
		m_defaultCamera = getCamera( id );
	}

	DirectX::XMFLOAT3 Scene::getAmbient() const
	{
		return DirectX::XMFLOAT3( m_lightData.ambientLight.x, m_lightData.ambientLight.y, m_lightData.ambientLight.z );
	}

	void Scene::setAmbient( const DirectX::XMFLOAT3 &color )
	{
		m_lightData.ambientLight = DirectX::XMFLOAT4( color.x, color.y, color.z, 1.0f );
	}
	void Scene::setAmbient( float r, float g, float b )
	{
		m_lightData.ambientLight = DirectX::XMFLOAT4( r, g, b, 1.0f );
	}

	void Scene::addDirectionalLight( const DirectX::XMFLOAT3 &direction, float intensity )
	{
		// push a Light struct in the vector
		m_lightData.lights.push_back( { direction, intensity } );
	}

	void Scene::addRenderableObject( const RenderableObject &obj )
	{
		m_renderObjects.push_back(obj);
	}


	void Scene::render( Camera *renderCam, const D3D11_VIEWPORT &vp, ID3D11RasterizerState *rs,
											ID3D11RenderTargetView *rtv, ID3D11DepthStencilView *dsv )
	{
		if ( !m_dxDeferredDevCont )
		{
			//TODO: change this so each worker thread has its own deferred context
			//		prob move this deferredContext creation to thread implementation
			// create deferred context
			ID3D11Device *dxDev = KGXCore::getInst()->getDxDevicePtr();
			dxDev->CreateDeferredContext( 0, &m_dxDeferredDevCont );
		}

		m_dxDeferredDevCont->RSSetViewports( 1, &vp );
		m_dxDeferredDevCont->RSSetState( rs );

		// render objects
		RenderPass mainRenderBucket( m_dxDeferredDevCont, rtv, dsv, renderCam->getViewMatrix(), renderCam->getProjMatrix(),
									 shaderProgPass1 );

		mainRenderBucket.record( m_renderObjects, m_lightData );
		mainRenderBucket.submit();
	}
}
