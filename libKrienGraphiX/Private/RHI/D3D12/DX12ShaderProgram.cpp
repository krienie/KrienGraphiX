
#include "DX12ShaderProgram.h"

#include "DX12GraphicsDevice.h"
#include "DX12MemoryUtils.h"

#include <d3dcompiler.h>
#include "d3dx12.h"

#include <cassert>
#include <iostream>

namespace
{
DXGI_FORMAT toDXGIFormat(kgx::VertexInputElementFormat rhiInputElementFormat)
{
    switch(rhiInputElementFormat)
    {
        case kgx::VertexInputElementFormat::FLOAT:
            return DXGI_FORMAT_R32_FLOAT;
        case kgx::VertexInputElementFormat::FLOAT2:
            return DXGI_FORMAT_R32G32_FLOAT;
        case kgx::VertexInputElementFormat::FLOAT3:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case kgx::VertexInputElementFormat::FLOAT4:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case kgx::VertexInputElementFormat::INT:
            return DXGI_FORMAT_R32_SINT;
        case kgx::VertexInputElementFormat::INT2:
            return DXGI_FORMAT_R32G32_SINT;
        case kgx::VertexInputElementFormat::INT3:
            return DXGI_FORMAT_R32G32B32_SINT;
        case kgx::VertexInputElementFormat::INT4:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        case kgx::VertexInputElementFormat::UINT:
            return DXGI_FORMAT_R32_UINT;
        case kgx::VertexInputElementFormat::UINT2:
            return DXGI_FORMAT_R32G32_UINT;
        case kgx::VertexInputElementFormat::UINT3:
            return DXGI_FORMAT_R32G32B32_UINT;
        case kgx::VertexInputElementFormat::UINT4:
            return DXGI_FORMAT_R32G32B32A32_UINT;
    }

    return DXGI_FORMAT_FORCE_UINT;
}

UINT getDXGISize(DXGI_FORMAT format)
{
    using namespace kgx::RHI;

    switch(format)
    {
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R32_UINT:
            return 4;
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G32_UINT:
            return 8;
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_SINT:
        case DXGI_FORMAT_R32G32B32_UINT:
            return 12;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
            return 16;
        default:
            //Unknown format size. Extend this switch-case to support other DXGI_FORMAT's
            assert(false);
    }

    return 0u;
}

}

namespace kgx::RHI
{
DX12ShaderProgram::DX12ShaderProgram()
    : RHIShaderProgram(), mDxDevice(nullptr), mConstantBuffers(), mInputDescs()
{
}

bool DX12ShaderProgram::init(RHIGraphicsDevice* device)
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

bool DX12ShaderProgram::loadCompiledShader(const CompiledShader & shaderDesc, ShaderType type)
{
    auto createAndFillShaderBlob = [](auto byteCode, auto blob)
    {
        D3DCreateBlob(byteCode.size(), &blob);
        memcpy(blob->GetBufferPointer(), byteCode.data(), byteCode.size());
    };

    switch (type)
    {
        case ShaderType::Vertex:
            createAndFillShaderBlob(shaderDesc.byteCode, mVertexShader.blob);
            mVertexShader.boundConstantBuffers = shaderDesc.boundConstantBuffers;
            break;
        case ShaderType::Hull:
            createAndFillShaderBlob(shaderDesc.byteCode, mHullShader.blob);
            mHullShader.boundConstantBuffers = shaderDesc.boundConstantBuffers;
            break;
        case ShaderType::Domain:
            createAndFillShaderBlob(shaderDesc.byteCode, mDomainShader.blob);
            mDomainShader.boundConstantBuffers = shaderDesc.boundConstantBuffers;
            break;
        case ShaderType::Geometry:
            createAndFillShaderBlob(shaderDesc.byteCode, mGeometryShader.blob);
            mGeometryShader.boundConstantBuffers = shaderDesc.boundConstantBuffers;
            break;
        case ShaderType::Pixel:
            createAndFillShaderBlob(shaderDesc.byteCode, mPixelShader.blob);
            mPixelShader.boundConstantBuffers = shaderDesc.boundConstantBuffers;
            break;
    }

    return true;
}

bool DX12ShaderProgram::loadConstantBuffers(const std::vector<ConstantBufferDescriptor>& bufferDescs)
{
    const UINT64 bufferHeapSize = [&bufferDescs]
    {
        UINT64 totalSize = 0u;
        for (auto & constBuffDesc : bufferDescs)
        {
            const auto alignedConstBufferSize = DX12MemoryUtils::alignTo256Bytes(constBuffDesc.size);
            totalSize += DX12MemoryUtils::alignTo64KBytes(alignedConstBufferSize);
        }

        return totalSize;
    }();

    auto *dxNativeDevice = mDxDevice->getNativeDevice();
    
    const CD3DX12_HEAP_DESC heapDesc(bufferHeapSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
    dxNativeDevice->CreateHeap(&heapDesc, IID_PPV_ARGS(&mConstantBufferHeap));

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
    cbvHeapDesc.NumDescriptors = static_cast<UINT>(bufferDescs.size());
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    dxNativeDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mConstBuffDescHeap));

    const auto cbvDescriptorSize = dxNativeDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle(mConstBuffDescHeap->GetCPUDescriptorHandleForHeapStart());

    UINT64 heapOffset = 0u;
    for (const auto & buffDesc : bufferDescs)
    {
        // Calculate current heap offset
        //TODO(KL): Optimize. This is already calculated above.
        const auto alignedConstBufferSize = DX12MemoryUtils::alignTo256Bytes(buffDesc.size);
        heapOffset += DX12MemoryUtils::alignTo64KBytes(alignedConstBufferSize);

        // create DX12ConstantBuffer
        DX12ConstantBufferDescriptor cbDesc
        {
            buffDesc.name,
            buffDesc.bufferRegister,
            buffDesc.size,
            mConstantBufferHeap,
            heapOffset,
            cbvHandle
        };

        mConstantBuffers.emplace_back(buffDesc.size, cbDesc);
        cbvHandle.Offset(1, cbvDescriptorSize);
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

bool DX12ShaderProgram::processShaderBlob(ID3DBlob *)
{
    auto * nativeDevice = mDxDevice->getNativeDevice();

    return false;

    // Create and commit resource to store the data in
    //HRESULT res = nativeDevice->CreateCommittedResource(
    //        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
    //        D3D12_HEAP_FLAG_NONE,
    //        &CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize),
    //        D3D12_RESOURCE_STATE_GENERIC_READ,
    //        nullptr,
    //        IID_PPV_ARGS(&mConstantBuffer));
    //
    //if (FAILED(res))
    //{
    //    return false;
    //}
}
}