
#include "Camera.h"
#include "RenderableObject.h"
#include "Scene.h"

namespace kgx
{
	Scene::Scene()
		: m_nextCamID(0u), m_defaultCamera(nullptr), m_cameras(), m_ambientLight( 0.25f, 0.25f, 0.25f, 1.0f ), m_lights(), m_renderObjects()
	{

	}

	Scene::~Scene()
	{
		std::map<CameraID, Camera*>::iterator camIt;
		for ( camIt = m_cameras.begin(); camIt != m_cameras.end(); ++camIt )
			delete camIt->second;

		std::vector<RenderableObject*>::iterator roIt;
		for ( roIt = m_renderObjects.begin(); roIt != m_renderObjects.end(); ++roIt )
			delete *roIt;
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
		return m_lights.cbegin();
	}
	Scene::const_lightiterator Scene::getLightCEnd() const
	{
		return m_lights.cend();
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
		return DirectX::XMFLOAT3( m_ambientLight.x, m_ambientLight.y, m_ambientLight.z );
	}

	void Scene::setAmbient( const DirectX::XMFLOAT3 &color )
	{
		m_ambientLight = DirectX::XMFLOAT4( color.x, color.y, color.z, 1.0f );
	}
	void Scene::setAmbient( float r, float g, float b )
	{
		m_ambientLight = DirectX::XMFLOAT4( r, g, b, 1.0f );
	}

	void Scene::addDirectionalLight( const DirectX::XMFLOAT3 &direction, float intensity, const std::string &name )
	{
		m_lights.push_back( Light(direction, intensity) );
	}


	void Scene::claimRenderableObject( RenderableObject *obj )
	{
		m_renderObjects.push_back(obj);
	}


	void Scene::render( Camera *renderCam )
	{
		//TODO: implement tiled/clustered shading

		std::vector<RenderableObject*>::iterator it;
		for ( it = m_renderObjects.begin(); it != m_renderObjects.end(); ++it )
			(*it)->draw( renderCam, m_lights, m_ambientLight );
	}
	void Scene::renderDefaultCam()
	{
		render( m_defaultCamera );
	}
}
