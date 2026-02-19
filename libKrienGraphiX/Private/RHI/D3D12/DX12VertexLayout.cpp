
#include "DX12VertexLayout.h"

#include <cassert>

namespace
{
DXGI_FORMAT toDxgiFormat(const kgx::VertexInputElementFormat& vertexInputElementFormat)
{
	switch (vertexInputElementFormat)
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

	// Unknown format. Extend this switch-case if any other VertexInputElementFormat's are added.
	assert(false);
	return DXGI_FORMAT_UNKNOWN;
}
}

namespace kgx::RHI
{
void DX12VertexLayout::addInputElement(const VertexInputElement& vertexInputElement)
{
	const DXGI_FORMAT dxgiFormat = toDxgiFormat(vertexInputElement.format);

	const D3D12_INPUT_ELEMENT_DESC inputDesc =
	{
		vertexInputElement.semanticName.c_str(),
		0,
		dxgiFormat,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0
	};

	mInputLayoutDesc.push_back(inputDesc);
}

void DX12VertexLayout::setFromInputElementVector(const std::vector<VertexInputElement>& vertexInputLayout)
{
	mInputLayoutDesc.clear();
	mInputLayoutDesc.reserve(vertexInputLayout.size());

	for (auto& vertexInputElement : vertexInputLayout)
	{
		addInputElement(vertexInputElement);
	}
}
}
