
#pragma once

#include "DX12Buffer.h"
#include "Private/RHI/RHIShader.h"

#include <d3d12.h>
#include <wrl\client.h>

namespace kgx::RHI
{
class DX12GraphicsDevice;
class DX12GraphicsCommandList;

class DX12Shader : public RHIShader
{
public:
    DX12Shader();
    ~DX12Shader() override = default;
    DX12Shader(DX12Shader& rhs) = default;
    DX12Shader(DX12Shader&& rhs) = default;
    DX12Shader& operator=(const DX12Shader& rhs) = default;
    DX12Shader& operator=(DX12Shader&& rhs) = default;

    bool create(RHIGraphicsDevice* device, RHIGraphicsCommandList* commandList, const CompiledShader& compiledShader, ShaderType type) override;
    
    void setVertexInputLayout(const std::vector<VertexInputElement>& vertexInputLayout) override;
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& getVertexInputLayout() const;

    [[nodiscard]] ID3DBlob* getShaderByteCode() const { return mLoadedShaderBlob.Get(); }
    [[nodiscard]] ID3D12RootSignature* getRootSignature() const { return mRootSignature.Get(); }

protected:
    bool loadConstantBuffers(const std::vector<ConstantBufferDescriptor> & bufferDescs) override;

private:
    bool createRootSignature(const CompiledShader& compiledShader);

    DX12GraphicsDevice* mDxDevice;
    DX12GraphicsCommandList* mDxCommandList;
    
    std::vector<DX12Buffer> mConstantBuffers;
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayoutDesc;

    Microsoft::WRL::ComPtr<ID3DBlob> mLoadedShaderBlob;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
    ShaderType mShaderType = ShaderType::Unassigned;
};
}
