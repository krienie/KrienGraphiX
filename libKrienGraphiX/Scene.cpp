
#include "Camera.h"
#include "RenderableObject.h"
#include "Scene.h"

namespace kgx
{
	Scene::Scene()
		: m_ambientLight(0.25f, 0.25f, 0.25f, 1.0f), m_lights(), m_renderObjects()
	{

	}

	Scene::~Scene()
	{
		std::vector<RenderableObject*>::iterator it;
		for ( it = m_renderObjects.begin(); it != m_renderObjects.end(); ++it )
			delete *it;
	}


	void Scene::addDirectionalLight( const DirectX::XMFLOAT3 &direction, float intensity )
	{
		m_lights.push_back( Light(direction, intensity) );
	}


	void Scene::claimRenderableObject( _In_ RenderableObject *obj )
	{
		m_renderObjects.push_back(obj);
	}


	void Scene::render( _In_ Camera *renderCam )
	{
		//TODO: collect list of visible lights to be used in the draw call below

		std::vector<RenderableObject*>::iterator it;
		for ( it = m_renderObjects.begin(); it != m_renderObjects.end(); ++it )
			(*it)->draw( renderCam, m_lights, m_ambientLight );
	}
}
