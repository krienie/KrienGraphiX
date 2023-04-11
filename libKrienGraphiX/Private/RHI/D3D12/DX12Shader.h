
#pragma once

#include "DX12ConstantBuffer.h"
#include "Private/RHI/RHIShader.h"

#include <d3d12.h>
#include <wrl\client.h>

namespace kgx::RHI
{
    class DX12GraphicsDevice;

    class DX12Shader : public RHIShader
{
    public:
        DX12Shader();
        ~DX12Shader() override = default;

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
        
        std::vector<DX12ConstantBuffer> mConstantBuffers;
        std::vector<D3D12_INPUT_ELEMENT_DESC> mInputDescs;

        DxShader mVertexShader;
        DxShader mHullShader;
        DxShader mDomainShader;
        DxShader mGeometryShader;
        DxShader mPixelShader;
};
}
