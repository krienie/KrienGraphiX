#include "Core/RenderThreadFence.h"

#include "Core/KGXCore.h"
#include "Core/RenderThread.h"

namespace kgx
{
    //TODO(KL): refactor into a function
    RenderThreadFence::RenderThreadFence()
        : m_done(false)
    {
        RenderThread* renderThread = KGXCore::get()->getRenderThread();

        renderThread->enqueueImmediateCommand([this](ID3D11DeviceContext*)
        {
            m_done = true;
        });

        while (!m_done)
        {
            std::this_thread::yield();
        }
    }
}
