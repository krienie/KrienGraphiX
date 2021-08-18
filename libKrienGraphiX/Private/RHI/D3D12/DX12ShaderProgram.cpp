
#include "DX12ShaderProgram.h"


#include <cassert>
#include <d3dcompiler.h>

#include "DX12GraphicsDevice.h"
#include "d3dx12.h"

#include <iostream>

namespace
{
DXGI_FORMAT toDXGIFormat(kgx::VertexInputElementFormat rhiInputElementFormat)
{
    switch(rhiInputElementFormat)
    {
        case kgx::VertexInputElementFormat::R_FLOAT:
            return DXGI_FORMAT_R32_FLOAT;
        case kgx::VertexInputElementFormat::RGB_FLOAT:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case kgx::VertexInputElementFormat::RGBA_FLOAT:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case kgx::VertexInputElementFormat::R_INT:
            return DXGI_FORMAT_R32_SINT;
        case kgx::VertexInputElementFormat::RGB_INT:
            return DXGI_FORMAT_R32G32B32_SINT;
        case kgx::VertexInputElementFormat::RGBA_INT:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        case kgx::VertexInputElementFormat::R_UINT:
            return DXGI_FORMAT_R32_UINT;
        case kgx::VertexInputElementFormat::RGB_UINT:
            return DXGI_FORMAT_R32G32B32_UINT;
        case kgx::VertexInputElementFormat::RGBA_UINT:
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

bool DX12ShaderProgram::loadCompiled(const decltype(kgx::CompiledShader::byteCode) & byteCode, ShaderType type)
{
    auto createAndFillShaderBlob = [](auto byteCode, auto blob)
    {
        D3DCreateBlob(byteCode.size(), &blob);
        memcpy(blob->GetBufferPointer(), byteCode.data(), byteCode.size());
    };

    switch (type)
    {
        case ShaderType::Vertex:
            createAndFillShaderBlob(byteCode, mVertexShader);
            break;
        case ShaderType::Hull:
            createAndFillShaderBlob(byteCode, mHullShader);
            break;
        case ShaderType::Domain:
            createAndFillShaderBlob(byteCode, mDomainShader);
            break;
        case ShaderType::Geometry:
            createAndFillShaderBlob(byteCode, mGeometryShader);
            break;
        case ShaderType::Pixel:
            createAndFillShaderBlob(byteCode, mPixelShader);
            break;
    }

    return true;
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