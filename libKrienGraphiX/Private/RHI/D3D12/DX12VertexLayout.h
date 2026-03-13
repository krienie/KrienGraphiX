
#pragma once

#include "Private/RHI/RHIVertexLayout.h"

#include "Private/RHI/RHIUtils.h"

#include <d3d12.h>

namespace kgx::RHI
{
class DX12VertexLayout : public RHIVertexLayout
{
public:
	DX12VertexLayout() = default;
	~DX12VertexLayout() override = default;

	void addInputElement(const VertexInputElement& vertexInputElement) override;
	void setFromInputElementVector(const std::vector<VertexInputElement>& vertexInputLayout) override;

	[[nodiscard]]
	std::vector<D3D12_INPUT_ELEMENT_DESC> getDX12VertexLayout() const { return mInputLayoutDesc; }

private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayoutDesc;
};

DEFINE_RESOURCE_CAST(DX12VertexLayout, RHIVertexLayout)
}
