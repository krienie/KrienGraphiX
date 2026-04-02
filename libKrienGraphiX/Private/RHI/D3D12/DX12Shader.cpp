
#include "DX12Shader.h"

#include "DX12GraphicsDevice.h"
#include "DX12GraphicsCommandList.h"

#include <d3dcompiler.h>
#include "d3dx12.h"

#include <cassert>

#include "DX12RenderHardwareInterface.h"
#include "DX12VertexLayout.h"
#include "Private/Core/RenderCore.h"

namespace kgx::RHI
{
bool DX12Shader::create(const CompiledShader& compiledShader, ShaderType type)
{
	mShaderType = type;

	// Load the compiled shader
	D3DCreateBlob(compiledShader.byteCode.size(), &mLoadedShaderBlob);
	memcpy(mLoadedShaderBlob->GetBufferPointer(), compiledShader.byteCode.data(), compiledShader.byteCode.size());

	return createRootSignature();
}

void DX12Shader::setVertexInputLayout(const std::vector<VertexInputElement>& vertexInputLayout)
{
	DX12VertexLayout dxVertexLayout;
	dxVertexLayout.setFromInputElementVector(vertexInputLayout);

	mInputLayoutDesc = dxVertexLayout.getDX12VertexLayout();
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>& DX12Shader::getVertexInputLayout() const
{
	return mInputLayoutDesc;
}

bool DX12Shader::createRootSignature()
{
	//TODO(KL): Create a root signature per frame

	ID3D12Device* nativeDevice = getDX12RHI()->getDX12Device()->getNativeDevice();

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

	if (FAILED(nativeDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		// Fallback to older version
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	//TODO(KL): Actually do something with the root signature feature data

	std::vector<CD3DX12_ROOT_PARAMETER> rootParameterSlots;
	// Only supporting CBV's for now, so we only need 1 root parameter slot
	rootParameterSlots.resize(1);
	rootParameterSlots[0].InitAsConstantBufferView(0);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
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

	const HRESULT result = nativeDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&mRootSignature));

	return SUCCEEDED(result);
}
}
