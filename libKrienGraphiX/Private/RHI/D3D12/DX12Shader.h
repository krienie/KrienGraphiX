
#pragma once

#include "Private/RHI/RHIUtils.h"
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
	DX12Shader() = default;
	~DX12Shader() override = default;

	bool create(const CompiledShader& compiledShader, ShaderType type) override;
	
	void setVertexInputLayout(const std::vector<VertexInputElement>& vertexInputLayout) override;
	const std::vector<D3D12_INPUT_ELEMENT_DESC>& getVertexInputLayout() const;

	[[nodiscard]] ID3DBlob* getShaderByteCode() const { return mLoadedShaderBlob.Get(); }
	[[nodiscard]] ID3D12RootSignature* getRootSignature() const { return mRootSignature.Get(); }

private:
	bool createRootSignature();

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayoutDesc;

	Microsoft::WRL::ComPtr<ID3DBlob> mLoadedShaderBlob;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
	ShaderType mShaderType = ShaderType::Unassigned;
};

DEFINE_RESOURCE_CAST(DX12Shader, RHIShader)
}
