
#include "Rendering/Passes/CompositionPass.h"

#include "Rendering/ShaderProgram.h"
#include "Rendering/TextureLibrary.h"

namespace kgx
{
    CompositionPass::CompositionPass( ID3D11DeviceContext *deferredDevCont, ID3D11DepthStencilView *dsv, ID3D11RenderTargetView *rtv )
        : RenderPass( deferredDevCont ), m_shaderProg( nullptr ), m_shaderProgLibrary( true ),
        m_depthStencilView( dsv ), m_renderTargetView( rtv )
    {
        m_shaderProg = m_shaderProgLibrary.getShaderProgram( "DeferredComposition" );
    }

    CompositionPass::~CompositionPass()
    {
    }

    void CompositionPass::record( const std::vector<RenderableObject*> &renderObjects, const LightData &lightData,
                                  const TextureLibrary &sceneTextures )
    {
        ID3D11DeviceContext *dxDeferredDevCont = getDeferredContext();

        dxDeferredDevCont->OMSetRenderTargets( 1, &m_renderTargetView, m_depthStencilView );

        // set default texture sampler
        ID3D11SamplerState *samplers[1] = { sceneTextures.getDefaultSampler() };
        dxDeferredDevCont->PSSetSamplers( 0u, 1u, samplers );

        // collect all ShaderResourceViews for all used textures
        std::vector<ID3D11ShaderResourceView*> texViews;
        sceneTextures.getShaderResourceViews( texViews );

        // set texture ShaderResourceViews for the pixel shader
        dxDeferredDevCont->PSSetShaderResources( 0u, static_cast<UINT>(texViews.size()), texViews.data() );

        // bind shaders to pipeline
        m_shaderProg->activate( dxDeferredDevCont );

        dxDeferredDevCont->IASetVertexBuffers( 0, 0, nullptr, nullptr, nullptr );
        dxDeferredDevCont->IASetIndexBuffer( nullptr, DXGI_FORMAT_R32_UINT, 0 );
        dxDeferredDevCont->IASetInputLayout( nullptr );
        dxDeferredDevCont->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

        // draw three vertices without a vertex buffer
        dxDeferredDevCont->Draw( 3u, 0u );

        finishCommandList();
    }
}
