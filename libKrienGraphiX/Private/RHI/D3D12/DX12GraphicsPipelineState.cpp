
#include "DX12GraphicsPipelineState.h"

#include "DX12GraphicsDevice.h"
#include "DX12Shader.h"

namespace
{
D3D12_SHADER_BYTECODE toD3D12ShaderBytecode(const kgx::RHI::DX12Shader* dxShader)
{
    ID3DBlob* shaderByteCode = dxShader->getShaderByteCode();

    return {
        static_cast<BYTE*>(shaderByteCode->GetBufferPointer()), 
        shaderByteCode->GetBufferSize() 
    };
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE toD3d12PrimitiveTopology(const kgx::RHI::RHIPrimitiveTopologyType& rhiTopologyType)
{
    using namespace kgx::RHI;

    switch (rhiTopologyType)
    {
    case Point:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    case Line:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case Triangle:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case Patch:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    case Undefined:
    default:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    }
}
}

namespace kgx::RHI
{
DX12GraphicsPipelineState::DX12GraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc)
    : RHIGraphicsPipelineState(desc), mPipelineState(nullptr)
{
}

bool DX12GraphicsPipelineState::create(RHIGraphicsDevice* device)
{
    auto* dxDevice = dynamic_cast<DX12GraphicsDevice*>(device);
    if (dxDevice == nullptr)
    {
        // This should never happen
        return false;
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

    if (mGraphicsDescriptor.VS)
    {
        const auto dxVertexShader = dynamic_cast<DX12Shader*>(mGraphicsDescriptor.VS);
        const auto& InputLayout = dxVertexShader->getVertexInputLayout();

        psoDesc.InputLayout = { InputLayout.data(), static_cast<UINT>(InputLayout.size()) };

        psoDesc.VS = toD3D12ShaderBytecode(dxVertexShader);
    }

    if (mGraphicsDescriptor.PS)
    {
        const auto dxPixelShader = dynamic_cast<DX12Shader*>(mGraphicsDescriptor.PS);
        psoDesc.PS = toD3D12ShaderBytecode(dxPixelShader);
        psoDesc.pRootSignature = dxPixelShader->getRootSignature();
    }

    //TODO(KL): implement other shader types

    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = toD3d12PrimitiveTopology(mGraphicsDescriptor.PrimitiveTopology);
    psoDesc.NumRenderTargets = mGraphicsDescriptor.NumRenderTargets;

    for (int i = 0; i < 8; ++i)
    {
        psoDesc.RTVFormats[i] = toDxgiPixelFormat(mGraphicsDescriptor.RenderTargetFormats[i]);
    }

    //TODO(KL): Implement MSAA support
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;

    psoDesc.DSVFormat = toDxgiPixelFormat(mGraphicsDescriptor.DepthStencilFormat);

    HRESULT res = dxDevice->getNativeDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState));

    return SUCCEEDED(res);
}

ID3D12PipelineState* DX12GraphicsPipelineState::getPSO() const
{
    return mPipelineState.Get();
}

} // namespace kgx::RHI