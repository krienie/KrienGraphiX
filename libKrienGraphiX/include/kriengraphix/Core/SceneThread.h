
#pragma once

#include <mutex>
#include <vector>

#include "Core/Commands/SceneCommand.h"
#include "Core/PersistentThread.h"
#include "Core/SceneView.h"
#include "Simulation/Scene.h"

namespace kgx
{
    class RenderWindow;
    class SceneObject;

class SceneThread final
{
    public:
        SceneThread() = default;
        ~SceneThread();
        SceneThread( const SceneThread& ) = delete;
        SceneThread& operator=( const SceneThread& ) = delete;

        void resetScene();
        void advance( double deltaTime );

        template<class Lambda>
        void enqueueSingleCommand(Lambda func)
        {
            m_commandThread.enqueueCommand<SceneCommand<Lambda>>(&m_mainScene, std::forward<Lambda>(func));
        }

        void flush();
        void addSceneView(RenderWindow *parentWindow);
        void addSceneObject(SceneObject *obj);

    private:
        PersistentThread m_commandThread;
        Scene m_mainScene;
        std::vector<SceneView*> m_sceneViews;

        std::mutex m_addMutex;
};
}
