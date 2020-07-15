
#include "Rendering/Passes/RenderPass.h"


#include <d3d11.h>
#include <iostream>


#include "Core/KGXCore.h"
#include "Core/RenderThread.h"
#include "Core/SceneView.h"

namespace kgx
{
    RenderPass::RenderPass(SceneView* view)
        : m_sceneView(view), m_commandList(KGXCore::get()->getRenderThread()->getDeferredRenderCommandList())
    {
        m_commandList.record([view](ID3D11DeviceContext* dxDeferredDevCont)
        {
            D3D11_VIEWPORT viewport = view->getViewport();
            dxDeferredDevCont->RSSetViewports(1, &viewport);
            dxDeferredDevCont->RSSetState(view->getRasterizerState());
        });
    }

    void RenderPass::submit() const
    {
        const long long curFrameNum = KGXCore::get()->getRenderThread()->getFrameNumber();
        KGXCore::get()->getRenderThread()->enqueueImmediateCommand([this, curFrameNum](ID3D11DeviceContext *dxDevCont)
        {
            m_commandList.submit(dxDevCont);
        });
    }

    SceneView *RenderPass::getSceneView() const
    {
        return m_sceneView;
    }

    DeferredRenderCommandList *RenderPass::getCommandList()
    {
        return &m_commandList;
    }

    void RenderPass::finishCommandList()
    {
        m_commandList.finish();
    }
}
