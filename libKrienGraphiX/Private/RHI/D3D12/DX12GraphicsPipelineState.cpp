
#include "DX12GraphicsPipelineState.h"

#include "DX12GraphicsDevice.h"
#include "DX12PixelFormat.h"
#include "DX12RenderHardwareInterface.h"
#include "DX12Shader.h"

namespace
{
D3D12_SHADER_BYTECODE toD3D12ShaderBytecode(const kgx::RHI::DX12Shader* dxShader)
{
	ID3DBlob* shaderByteCode = dxShader->getShaderByteCode();

	return {
		static_cast<BYTE*>(shaderByteCode->GetBufferPointer()), 
		shaderByteCode->GetBufferSize() 
	};
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE toD3d12PrimitiveTopology(const kgx::RHI::RHIPrimitiveTopologyType& rhiTopologyType)
{
	using namespace kgx::RHI;

	switch (rhiTopologyType)
	{
	case Point:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	case Line:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	case Triangle:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	case Patch:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	case Undefined:
	default:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}
}
}

namespace kgx::RHI
{
DX12GraphicsPipelineState::DX12GraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc)
	: RHIGraphicsPipelineState(desc), mPipelineState(nullptr)
{
}

bool DX12GraphicsPipelineState::create()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	if (mGraphicsDescriptor.vs)
	{
		const auto dxVertexShader = static_cast<DX12Shader*>(mGraphicsDescriptor.vs);
		const auto& InputLayout = dxVertexShader->getVertexInputLayout();

		psoDesc.InputLayout = { InputLayout.data(), static_cast<UINT>(InputLayout.size()) };

		psoDesc.VS = toD3D12ShaderBytecode(dxVertexShader);
		psoDesc.pRootSignature = dxVertexShader->getRootSignature();
	}

	if (mGraphicsDescriptor.ps)
	{
		const auto dxPixelShader = static_cast<DX12Shader*>(mGraphicsDescriptor.ps);
		psoDesc.PS = toD3D12ShaderBytecode(dxPixelShader);
		psoDesc.pRootSignature = dxPixelShader->getRootSignature();
	}

	//TODO(KL): implement other shader types
	//TODO(KL): Create root signature based on PSO here?

	mRootSignature = psoDesc.pRootSignature;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = toD3d12PrimitiveTopology(mGraphicsDescriptor.primitiveTopology);
	psoDesc.NumRenderTargets = mGraphicsDescriptor.numRenderTargets;

	for (unsigned int i = 0; i < mGraphicsDescriptor.numRenderTargets; ++i)
	{
		psoDesc.RTVFormats[i] = toDxgiPixelFormat(mGraphicsDescriptor.renderTargetFormats[i]);
	}

	//TODO(KL): Implement MSAA support
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;

	psoDesc.DSVFormat = toDxgiPixelFormat(mGraphicsDescriptor.depthStencilFormat);

	ID3D12Device* nativeDevice = getDX12RHI()->getDX12Device()->getNativeDevice();
	HRESULT res = nativeDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState));

	return SUCCEEDED(res);
}

ID3D12PipelineState* DX12GraphicsPipelineState::getPSO() const
{
	return mPipelineState.Get();
}

ID3D12RootSignature* DX12GraphicsPipelineState::getRootSignature() const
{
	return mRootSignature;
}
} // namespace kgx::RHI