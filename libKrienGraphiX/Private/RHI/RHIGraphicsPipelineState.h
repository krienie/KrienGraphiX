
#pragma once

#include <memory>

namespace kgx::RHI
{
class RHIGraphicsDevice;
class RHIShaderProgram;

/*
typedef struct D3D12_GRAPHICS_PIPELINE_STATE_DESC {
  ID3D12RootSignature                *pRootSignature;       // uit shaderprogram
  D3D12_SHADER_BYTECODE              VS;                    // uit shaderprogram
  D3D12_SHADER_BYTECODE              PS;                    // uit shaderprogram
  D3D12_SHADER_BYTECODE              DS;                    // uit shaderprogram
  D3D12_SHADER_BYTECODE              HS;                    // uit shaderprogram
  D3D12_SHADER_BYTECODE              GS;                    // uit shaderprogram
  D3D12_STREAM_OUTPUT_DESC           StreamOutput;          // unsupported for now
  D3D12_BLEND_DESC                   BlendState;
  UINT                               SampleMask;            // samen met blendState een functie
  D3D12_RASTERIZER_DESC              RasterizerState;       // losse functie
  D3D12_DEPTH_STENCIL_DESC           DepthStencilState;     // losse functie
  D3D12_INPUT_LAYOUT_DESC            InputLayout;           // uit shaderprogram
  D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;       // specifiek voor vertex strips. Unsupported for now
  D3D12_PRIMITIVE_TOPOLOGY_TYPE      PrimitiveTopologyType; // losse functie
  UINT                               NumRenderTargets;
  DXGI_FORMAT                        RTVFormats[8];         // losse functie
  DXGI_FORMAT                        DSVFormat;             // Gezet door depth stencil desc functie
  DXGI_SAMPLE_DESC                   SampleDesc;
  UINT                               NodeMask;              // Gebruikt voor multiple GPU systems. Unsupported.
  D3D12_CACHED_PIPELINE_STATE        CachedPSO;             // Evt pipeline cache. Unsupported for now. To be designed.
  D3D12_PIPELINE_STATE_FLAGS         Flags;                 // Unsupported. Always set to NONE.
} D3D12_GRAPHICS_PIPELINE_STATE_DESC;
     */
class RHIGraphicsPipelineState
{
public:
    virtual ~RHIGraphicsPipelineState() = default;

    virtual bool construct(RHIGraphicsDevice * device) = 0;

    void setShaderProgram(const std::shared_ptr<RHIShaderProgram> & shaderProgram) { mShaderProgram = shaderProgram; }

    //TODO(KL): add interface to initialize and modify the pipeline state

private:
    std::shared_ptr<RHIShaderProgram> mShaderProgram;
};
}