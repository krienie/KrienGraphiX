
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

bool DX12Shader::create(RHIGraphicsDevice* device, const CompiledShader& compiledShader, ShaderType type)
{
    auto* dxDevice = dynamic_cast<DX12GraphicsDevice*>(device);
    if (dxDevice == nullptr)
    {
        // This should never happen
        return false;
    }

    mDxDevice = dxDevice;

    mShaderType = type;

    // Load the compiled shader
    D3DCreateBlob(compiledShader.byteCode.size(), &mLoadedShaderBlob);
    memcpy(mLoadedShaderBlob->GetBufferPointer(), compiledShader.byteCode.data(), compiledShader.byteCode.size());

    const bool success = loadConstantBuffers(compiledShader.constantBuffers);
    if (!success)
    {
        return false;
    }

    return createRootSignature(compiledShader);
}

void DX12Shader::setVertexInputLayout(const std::vector<VertexInputElement>& vertexInputLayout)
{
    //TODO(KL): DX12VertexLayout can prob be just a static method if it's only used like this
    DX12VertexLayout dxVertexLayout;
    dxVertexLayout.setFromInputElementVector(vertexInputLayout);

    mInputLayoutDesc = dxVertexLayout.getDX12VertexLayout();
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>& DX12Shader::getVertexInputLayout() const
{
    return mInputLayoutDesc;
}

//TODO(KL): Misschien is dit niet te juiste plek om ConstantBuffers te zetten. Verplaatsen naar RenderPass?
bool DX12Shader::loadConstantBuffers(const std::vector<ConstantBufferDescriptor>& bufferDescs)
{
    for (const auto & buffDesc : bufferDescs)
    {
        // create DX12ConstantBuffer
        RHIConstantBufferDescriptor cbDesc
        {
            buffDesc.name,
            buffDesc.bufferRegister,
            buffDesc.size,
            RHIResource::ShaderResource
        };
        
        mConstantBuffers.emplace_back(mDxDevice, cbDesc);
    }

    return true;
}

bool DX12Shader::createRootSignature(const CompiledShader& compiledShader)
{
    auto *dxNativeDevice = mDxDevice->getNativeDevice();

    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(dxNativeDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        // Fallback to older version
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    //TODO(KL): Actually do something with the root signature feature data

    // Only supporting CBV's for now, so we only need 1 root parameter slot
    std::vector<CD3DX12_ROOT_PARAMETER> rootParameterSlots(1);

    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;

    CD3DX12_DESCRIPTOR_RANGE cbvTable;
    cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, static_cast<UINT>(compiledShader.constantBuffers.size()), 0);
    rootParameterSlots[0].InitAsDescriptorTable(1, &cbvTable);

    rootSigDesc.Init(static_cast<UINT>(rootParameterSlots.size()), rootParameterSlots.data(), 0, nullptr,
                     D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    D3D12SerializeRootSignature(&rootSigDesc, featureData.HighestVersion,
                                             serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
        return false;
    }

    const HRESULT result = dxNativeDevice->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignature));

    return SUCCEEDED(result);
}
}