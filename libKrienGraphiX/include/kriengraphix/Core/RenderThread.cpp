#include "Core/RenderThread.h"

#include "Core/RenderThreadFence.h"

namespace kgx
{
    
DeferredRenderCommandList::DeferredRenderCommandList(std::shared_ptr<ID3D11DeviceContext> deferredDevCont)
    : m_dxDeferredDevCont(std::move(deferredDevCont)), m_commandList(nullptr)
{
}

DeferredRenderCommandList::~DeferredRenderCommandList()
{
    if (m_commandList)
    {
        m_commandList->Release();
    }
}

void DeferredRenderCommandList::finish()
{
    // Release any previously created commandlist
    if ( m_commandList )
    {
        m_commandList->Release();
        m_commandList = nullptr;
    }

    m_dxDeferredDevCont->FinishCommandList(false, &m_commandList);
    m_dxDeferredDevCont->ClearState();
}

void DeferredRenderCommandList::submit(ID3D11DeviceContext *immediateDevCont) const
{
    if (m_commandList)
    {
        immediateDevCont->ExecuteCommandList(m_commandList, false);
    }
}

RenderThread::RenderThread(ID3D11Device *dxDev)
    : m_dxDev(dxDev), m_dxDevCont(nullptr), m_frameNum(0)
{
    m_dxDev->GetImmediateContext(&m_dxDevCont);
};
RenderThread::~RenderThread()
{
    m_dxDevCont->Release();

    for (const auto &deferredDevCont : m_deferredDevContexts)
    {
        deferredDevCont->Release();
    }
}

void RenderThread::flush()
{
    RenderThreadFence fence;
}

DeferredRenderCommandList RenderThread::getDeferredRenderCommandList()
{
    for (const auto &deferredDevCont : m_deferredDevContexts)
    {
        if (deferredDevCont.use_count() == 1)
        {
            return DeferredRenderCommandList(deferredDevCont);
        }
    }

    // No free deferred context available. Create a new one and add it to the list.
    ID3D11DeviceContext *deferredDevContext;
    m_dxDev->CreateDeferredContext(0, &deferredDevContext);
    m_deferredDevContexts.emplace_back(deferredDevContext, [](ID3D11DeviceContext *defDevCont) { defDevCont->Release(); });

    return DeferredRenderCommandList(m_deferredDevContexts.back());
}

long long RenderThread::getFrameNumber() const
{
    return m_frameNum;
}

void RenderThread::incrementFrame()
{
    ++m_frameNum;
}
}
