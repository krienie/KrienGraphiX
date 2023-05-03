
#include "DX12Shader.h"

#include "DX12GraphicsDevice.h"
#include "DX12MemoryUtils.h"

#include <d3dcompiler.h>
#include "d3dx12.h"

#include <cassert>
#include <iostream>

#include "DX12VertexLayout.h"

namespace kgx::RHI
{
DX12Shader::DX12Shader()
    : RHIShader(), mDxDevice(nullptr), mConstantBuffers(), mInputLayoutDesc()
{
}

bool DX12Shader::init(RHIGraphicsDevice* device, [[maybe_unused]] const CompiledShader& compiledShader, [[maybe_unused]] ShaderType type)
{
    auto * dxDevice = dynamic_cast<DX12GraphicsDevice*>(device);
    if (dxDevice == nullptr)
    {
        // This should never happen
        return false;
    }

    mDxDevice = dxDevice;

    return true;
}

void DX12Shader::setVertexInputLayout(const std::vector<VertexInputElement>& vertexInputLayout)
{
    //TODO(KL): DX12VertexLayout can prob be just a static method if it's only used like this
    DX12VertexLayout dxVertexLayout;
    dxVertexLayout.setFromInputElementVector(vertexInputLayout);

    mInputLayoutDesc = dxVertexLayout.getDX12VertexLayout();
}

bool DX12Shader::loadCompiledShader([[maybe_unused]] const CompiledShader& shaderDesc, [[maybe_unused]] ShaderType type)
{
    auto createAndFillShaderBlob = [](auto byteCode, auto blob)
    {
        D3DCreateBlob(byteCode.size(), &blob);
        memcpy(blob->GetBufferPointer(), byteCode.data(), byteCode.size());
    };

    //switch (type)
    //{
    //    case ShaderType::Vertex:
    //        createAndFillShaderBlob(shaderDesc.byteCode, mVertexShader.blob);
    //        mVertexShader.boundConstantBuffers = shaderDesc.boundConstantBuffers;
    //        break;
    //    case ShaderType::Hull:
    //        createAndFillShaderBlob(shaderDesc.byteCode, mHullShader.blob);
    //        mHullShader.boundConstantBuffers = shaderDesc.boundConstantBuffers;
    //        break;
    //    case ShaderType::Domain:
    //        createAndFillShaderBlob(shaderDesc.byteCode, mDomainShader.blob);
    //        mDomainShader.boundConstantBuffers = shaderDesc.boundConstantBuffers;
    //        break;
    //    case ShaderType::Geometry:
    //        createAndFillShaderBlob(shaderDesc.byteCode, mGeometryShader.blob);
    //        mGeometryShader.boundConstantBuffers = shaderDesc.boundConstantBuffers;
    //        break;
    //    case ShaderType::Pixel:
    //        createAndFillShaderBlob(shaderDesc.byteCode, mPixelShader.blob);
    //        mPixelShader.boundConstantBuffers = shaderDesc.boundConstantBuffers;
    //        break;
    //}

    return true;
}

bool DX12Shader::loadConstantBuffers(const std::vector<ConstantBufferDescriptor>& bufferDescs)
{
    //const UINT64 bufferHeapSize = [&bufferDescs]
    //{
    //    UINT64 totalSize = 0u;
    //    for (auto & constBuffDesc : bufferDescs)
    //    {
    //        const auto alignedConstBufferSize = DX12MemoryUtils::alignTo256Bytes(constBuffDesc.size);
    //        totalSize += DX12MemoryUtils::alignTo64KBytes(alignedConstBufferSize);
    //    }
    //
    //    return totalSize;
    //}();

    //auto *dxNativeDevice = mDxDevice->getNativeDevice();
    //
    //const CD3DX12_HEAP_DESC heapDesc(bufferHeapSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
    //dxNativeDevice->CreateHeap(&heapDesc, IID_PPV_ARGS(&mConstantBufferHeap));
    //
    //D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
    //cbvHeapDesc.NumDescriptors = static_cast<UINT>(bufferDescs.size());
    //cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    //cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    //dxNativeDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mConstBuffDescHeap));
    //
    //const auto cbvDescriptorSize = dxNativeDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    //
    //CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle(mConstBuffDescHeap->GetCPUDescriptorHandleForHeapStart());

    //UINT64 heapOffset = 0u;
    for (const auto & buffDesc : bufferDescs)
    {
        // Calculate current heap offset
        //TODO(KL): Optimize. This is already calculated above.
        const auto alignedConstBufferSize = DX12MemoryUtils::alignTo256Bytes(buffDesc.size);
        //heapOffset += DX12MemoryUtils::alignTo64KBytes(alignedConstBufferSize);

        // create DX12ConstantBuffer
        RHIConstantBufferDescriptor cbDesc
        {
            buffDesc.name,
            buffDesc.bufferRegister,
            buffDesc.size,
            RHIResource::ShaderResource
        };

        //mConstantBuffers.emplace_back(buffDesc.size, cbDesc);
        //cbvHandle.Offset(1, cbvDescriptorSize);
    }

    return false;
}

bool DX12Shader::createRootSignature()
{
    auto *dxNativeDevice = mDxDevice->getNativeDevice();

    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(dxNativeDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        // Fallback to older version
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }



    return false;
}

//void DX12ShaderProgram::loadFromFile(const std::string &blobFile, ShaderType type)
//{
//    std::wstring wShaderFile = std::wstring( blobFile.begin(), blobFile.end() );
//
//    // Try to load pre-compiled shader file
//    ID3DBlob *shaderSource;
//    HRESULT res = D3DReadFileToBlob( wShaderFile.c_str(), &shaderSource );
//    if ( FAILED( res ) )
//    {
//        std::cout << "Error (RcShaderProgram::loadFromBlob): Error loading shader source " << wShaderFile.c_str() << std::endl;
//        return;
//    }
//
//
//}

/*bool DX12ShaderProgram::compile(const kgx::ShaderProgramDescriptor &shaderDesc)
{
    // Create Input Layout
    UINT byteOffset = 0u;
    mInputDescs.reserve(shaderDesc.InputLayout.size());
    for (const auto & inputDesc : shaderDesc.InputLayout)
    {
        const auto dxgiFormat = toDXGIFormat(inputDesc.format);
        mInputDescs.push_back(
            {inputDesc.semanticName.c_str(), 0u, dxgiFormat, 0u, byteOffset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u});

        byteOffset += getDXGISize(dxgiFormat);
    }

    auto loadShaderBlob = [](const std::string &filePath, Microsoft::WRL::ComPtr<ID3DBlob> &dest)
    {
        if (!filePath.empty())
        {
            return;
        }

        const std::wstring wShaderFile = std::wstring(filePath.begin(), filePath.end());
        HRESULT res = D3DReadFileToBlob(wShaderFile.c_str(), &dest);
        if ( FAILED( res ) )
        {
            //std::cout << "Error (RcShaderProgram::loadFromBlob): Error loading shader source " << wShaderFile.c_str() << std::endl;
            //TODO(KL): log error
        }

        //TODO(KL): call processShaderBlob to create all constant buffers and resource views
    };

    // Try to load all shaders
    loadShaderBlob(shaderDesc.VertexShaderPath, mVertexShader);
    loadShaderBlob(shaderDesc.HullShaderPath, mHullShader);
    loadShaderBlob(shaderDesc.DomainShaderPath, mDomainShader);
    loadShaderBlob(shaderDesc.GeometryShaderPath, mGeometryShader);
    loadShaderBlob(shaderDesc.PixelShaderPath, mPixelShader);

    return true;
}*/
}