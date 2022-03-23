
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
        bool setVertexInputLayout(/*define*/) override;
        bool loadCompiledShader(const CompiledShader & shaderDesc, ShaderType type) override;
        bool loadConstantBuffers(const std::vector<ConstantBufferDescriptor> & bufferDescs) override;

        //[[nodiscard]] ID3DBlob* getVertexShader() const { return mVertexShader.blob.Get(); }
        //[[nodiscard]] ID3DBlob* getHullShader() const { return mHullShader.blob.Get(); }
        //[[nodiscard]] ID3DBlob* getDomainShader() const { return mDomainShader.blob.Get(); }
        //[[nodiscard]] ID3DBlob* getGeometryShader() const { return mGeometryShader.blob.Get(); }
        //[[nodiscard]] ID3DBlob* getPixelShader() const { return mPixelShader.blob.Get(); }

    private:
        struct DxShader
        {
            Microsoft::WRL::ComPtr<ID3DBlob> blob;
            std::vector<int> boundConstantBuffers;
        };

        bool createRootSignature();

        DX12GraphicsDevice* mDxDevice;

        Microsoft::WRL::ComPtr<ID3D12Heap> mConstantBufferHeap;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mConstBuffDescHeap;
        std::vector<DX12ConstantBuffer> mConstantBuffers;
        std::vector<D3D12_INPUT_ELEMENT_DESC> mInputDescs;

        DxShader mVertexShader;
        DxShader mHullShader;
        DxShader mDomainShader;
        DxShader mGeometryShader;
        DxShader mPixelShader;
};
}
