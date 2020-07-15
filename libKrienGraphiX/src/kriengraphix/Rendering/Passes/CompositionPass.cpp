
#include "Rendering/Passes/CompositionPass.h"

#include "Core/RenderThread.h"
#include "Core/SceneView.h"
#include "Rendering/ShaderProgram.h"
#include "Rendering/ShaderProgramLibrary.h"
#include "Rendering/TextureLibrary.h"

namespace kgx
{
    CompositionPass::CompositionPass(SceneView *view)
        : RenderPass(view)
    {
    }
    void CompositionPass::record(const LightData &lightData, ShaderProgramLibrary &shaderLibrary, TextureLibrary &sceneTextures)
    {
        SceneView *view = getSceneView();
        if (!view)
        {
            // Nothing to do if we don't have a valid SceneView
            return;
        }

        DeferredRenderCommandList *commandList = getCommandList();

        unsigned int tmpW = view->getWidth();

        ID3D11RenderTargetView *rtv = view->getRenderTarget();
        ID3D11DepthStencilView *dsv = view->getDepthStencil();
        commandList->record([rtv, dsv](ID3D11DeviceContext* dxDeferredDevCont)
        {
            dxDeferredDevCont->OMSetRenderTargets(1, &rtv, dsv);
        });

        // set default texture sampler
        ID3D11SamplerState *samplers[1] = { sceneTextures.getDefaultSampler() };
        commandList->record([samplers](ID3D11DeviceContext* dxDeferredDevCont)
        {
            dxDeferredDevCont->PSSetSamplers(0u, 1u, samplers);
        });

        // collect all ShaderResourceViews for all used textures
        std::vector<ID3D11ShaderResourceView*> texViews;
        sceneTextures.getShaderResourceViews( texViews );

        // set texture ShaderResourceViews for the pixel shader
        commandList->record([texViews](ID3D11DeviceContext* dxDeferredDevCont)
        {
            dxDeferredDevCont->PSSetShaderResources(0u, static_cast<UINT>(texViews.size()), texViews.data());
        });

        // bind shaders to pipeline
        ShaderProgram *gbuffProg = shaderLibrary.getShaderProgram("DeferredComposition");
        commandList->record([gbuffProg](ID3D11DeviceContext* dxDeferredDevCont)
        {
           gbuffProg->activate(dxDeferredDevCont);
        });

        commandList->record([](ID3D11DeviceContext* dxDeferredDevCont)
        {
            dxDeferredDevCont->IASetVertexBuffers( 0, 0, nullptr, nullptr, nullptr );
            dxDeferredDevCont->IASetIndexBuffer( nullptr, DXGI_FORMAT_R32_UINT, 0 );
            dxDeferredDevCont->IASetInputLayout( nullptr );
            dxDeferredDevCont->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

            // draw three vertices without a vertex buffer
            dxDeferredDevCont->Draw( 3u, 0u );
        });

        finishCommandList();
    }
}
