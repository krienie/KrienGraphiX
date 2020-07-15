
#pragma once

#include <atomic>
#include <memory>
#include <vector>

#include <d3d11.h>

#include "Core/Commands/RenderCommand.h"
#include "Core/PersistentThread.h"

namespace kgx
{
class DeferredRenderCommandList final
{
public:
    DeferredRenderCommandList(std::shared_ptr<ID3D11DeviceContext> deferredDevCont);
    ~DeferredRenderCommandList();
    DeferredRenderCommandList(const DeferredRenderCommandList &rhs) noexcept = delete;
    DeferredRenderCommandList& operator=(const DeferredRenderCommandList &rhs) noexcept = delete;
    DeferredRenderCommandList(DeferredRenderCommandList &&rhs) noexcept = default;
    DeferredRenderCommandList& operator=(DeferredRenderCommandList &&rhs) noexcept = default;

    template<class Lambda>
    void record(Lambda func)
    {
        func(m_dxDeferredDevCont.get());
    }

    void finish();
    void submit(ID3D11DeviceContext *immediateDevCont) const;

private:
    std::shared_ptr<ID3D11DeviceContext> m_dxDeferredDevCont;
    ID3D11CommandList *m_commandList;
};

class RenderThread final
{
    public:
        RenderThread(ID3D11Device *dxDev);
        ~RenderThread();

        RenderThread(const RenderThread&) noexcept            = delete;
        RenderThread& operator=(const RenderThread&) noexcept = delete;
        RenderThread(RenderThread&&) noexcept                 = delete;
        RenderThread& operator=(RenderThread&&) noexcept      = delete;

        template<class Lambda>
        void enqueueImmediateCommand(Lambda func)
        {
            m_commandThread.enqueueCommand<RenderCommand<Lambda>>(m_dxDevCont, std::forward<Lambda>(func));
        }

        void flush();

        DeferredRenderCommandList getDeferredRenderCommandList();

        [[nodiscard]]
        long long getFrameNumber() const;
        void incrementFrame();

    private:
        ID3D11Device *m_dxDev;
        ID3D11DeviceContext *m_dxDevCont;
        std::vector<std::shared_ptr<ID3D11DeviceContext>> m_deferredDevContexts;
        PersistentThread m_commandThread;

        std::atomic<long long> m_frameNum;
};
}
