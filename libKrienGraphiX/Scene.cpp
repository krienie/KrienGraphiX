
#include "Camera.h"
#include "RenderableObject.h"
#include "Scene.h"

namespace kgx
{
	Scene::Scene()
		: renderObjects()
	{

	}

	Scene::~Scene()
	{
		std::vector<RenderableObject*>::iterator it;
		for ( it = renderObjects.begin(); it != renderObjects.end(); ++it )
			delete *it;
	}


	void Scene::claimRenderableObject( _In_ RenderableObject *obj )
	{
		renderObjects.push_back(obj);
	}


	void Scene::render( _In_ Camera *renderCam )
	{
		// activate lights

		// for every RenderableObject
		//		draw RenderableObject

		std::vector<RenderableObject*>::iterator it;
		for ( it = renderObjects.begin(); it != renderObjects.end(); ++it )
			(*it)->draw( renderCam );
	}
}
