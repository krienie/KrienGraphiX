#include "Core/SceneThreadFence.h"

#include "Core/KGXCore.h"
#include "Core/SceneThread.h"

namespace kgx
{
    SceneThreadFence::SceneThreadFence()
        : m_done(false)
    {
        SceneThread* sceneThread = KGXCore::get()->getSceneThread();

        sceneThread->enqueueSingleCommand([this](Scene*)
        {
            m_done = true;
        });

        while (!m_done)
        {
            std::this_thread::yield();
        }
    }
}
