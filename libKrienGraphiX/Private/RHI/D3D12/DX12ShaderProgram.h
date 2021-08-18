
#pragma once

#include "DX12ConstantBuffer.h"
#include "Private/RHI/RHIShaderProgram.h"

#include <d3d12.h>
#include <wrl\client.h>

namespace kgx::RHI
{
    class DX12GraphicsDevice;

    class DX12ShaderProgram : public RHIShaderProgram
{
    public:
        DX12ShaderProgram();
        ~DX12ShaderProgram() override = default;

        bool init(RHIGraphicsDevice *device) override;

        //bool compile(const kgx::ShaderProgramDescriptor &shaderDesc) override;
        bool loadCompiled(const decltype(kgx::CompiledShader::byteCode) & byteCode, ShaderType type) override;

        [[nodiscard]] ID3DBlob * getVertexShader() const { return mVertexShader.Get(); }
        [[nodiscard]] ID3DBlob * getHullShader() const { return mHullShader.Get(); }
        [[nodiscard]] ID3DBlob * getDomainShader() const { return mDomainShader.Get(); }
        [[nodiscard]] ID3DBlob * getGeometryShader() const { return mGeometryShader.Get(); }
        [[nodiscard]] ID3DBlob * getPixelShader() const { return mPixelShader.Get(); }

    private:
        bool processShaderBlob(ID3DBlob *shaderBlob);

        DX12GraphicsDevice * mDxDevice;

        Microsoft::WRL::ComPtr<ID3D12Heap> mConstantBufferHeap;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap;
        std::vector<DX12ConstantBuffer> mConstantBuffers;
        std::vector<D3D12_INPUT_ELEMENT_DESC> mInputDescs;

        Microsoft::WRL::ComPtr<ID3DBlob> mVertexShader;
        Microsoft::WRL::ComPtr<ID3DBlob> mHullShader;
        Microsoft::WRL::ComPtr<ID3DBlob> mDomainShader;
        Microsoft::WRL::ComPtr<ID3DBlob> mGeometryShader;
        Microsoft::WRL::ComPtr<ID3DBlob> mPixelShader;
};
}
