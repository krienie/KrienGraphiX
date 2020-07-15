
#include "Core/SceneThread.h"

#include "Core/PhysXManager.h"
#include "Core/SceneThreadFence.h"
#include "Core/SceneView.h"
#include "Rendering/Renderer.h"
#include "Simulation/Scene.h"

namespace kgx
{
    SceneThread::~SceneThread()
    {
        for (auto &sceneView : m_sceneViews)
        {
            delete sceneView;
        }
    }

    void SceneThread::resetScene()
    {
        SceneThreadFence fence;
        //TODO(KL): do actual scene resetting here...
    }

    void SceneThread::advance(double deltaTime)
    {
        std::unique_lock<std::mutex> lock(m_addMutex);

        // TODO: update frame listeners

        //kgx::PhysXManager::getInst()->advance( static_cast<float>(deltaTime) );

        for (auto &sceneView : m_sceneViews)
        {
            enqueueSingleCommand([sceneView, this](Scene*)
            {
                Renderer renderer;
                renderer.render(&m_mainScene, sceneView);
            });
        }

        // TODO: enqueue render call for current scene state
    }

    void SceneThread::flush()
    {
        SceneThreadFence fence;
    }

    void SceneThread::addSceneView(RenderWindow *parentWindow)
    {
        std::unique_lock<std::mutex> lock(m_addMutex);
        m_sceneViews.push_back(new SceneView(parentWindow));
    }

    void SceneThread::addSceneObject(SceneObject *obj)
    {
        std::unique_lock<std::mutex> lock(m_addMutex);
        m_mainScene.addSceneObject(obj);
    }
}
