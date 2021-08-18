
#include "DX12GraphicsPipelineState.h"

#include "DX12GraphicsDevice.h"
#include "DX12ShaderProgram.h"

namespace kgx::RHI
{
DX12GraphicsPipelineState::DX12GraphicsPipelineState()
    : RHIGraphicsPipelineState(), mPipelineState(nullptr)
{}

bool DX12GraphicsPipelineState::construct(RHIGraphicsDevice * device)
{
    //auto * dxShaderProgram = dynamic_cast<DX12ShaderProgram*>(shaderProgram);
    //if (dxShaderProgram == nullptr)
    //{
    //    //TODO(KL): error handling?
    //    return;
    //}

    return false;
}

ID3D12PipelineState * DX12GraphicsPipelineState::getPipelineState() const
{
    return mPipelineState.Get();
}

void DX12GraphicsPipelineState::setShaderProgram(RHIShaderProgram * shaderProgram)
{
    ID3D12RootSignature                *pRootSignature;       // uit shaderprogram
    D3D12_SHADER_BYTECODE              VS;                    // uit shaderprogram
    D3D12_SHADER_BYTECODE              PS;                    // uit shaderprogram
    D3D12_SHADER_BYTECODE              DS;                    // uit shaderprogram
    D3D12_SHADER_BYTECODE              HS;                    // uit shaderprogram
    D3D12_SHADER_BYTECODE              GS;                    // uit shaderprogram
    D3D12_INPUT_LAYOUT_DESC            InputLayout;           // uit shaderprogram
}

} // namespace kgx::RHI