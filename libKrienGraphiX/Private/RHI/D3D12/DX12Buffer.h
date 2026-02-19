
#pragma once

#include "DX12Resource.h"
#include "Private/RHI/RHIBuffer.h"

namespace kgx::RHI
{
class DX12Buffer final : public RHIBuffer
{
public:
	DX12Buffer(DX12GraphicsCommandList* commandList, const RHIBufferDescriptor& descriptor);
	~DX12Buffer() override = default;

	DX12Buffer(DX12Buffer&) = delete;
	DX12Buffer(DX12Buffer&&) noexcept = default;
	DX12Buffer& operator=(DX12Buffer&) = delete;
	DX12Buffer& operator=(DX12Buffer&&) noexcept = default;

	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource> getResource() const;

	const D3D12_INDEX_BUFFER_VIEW* getIndexBufferView() const;
	const D3D12_VERTEX_BUFFER_VIEW* getVertexBufferView() const;

private:
	void* mapImpl(MapType type) override;
	void unmapImpl() override;
	
	RHIBufferDescriptor mDescriptor;
	Microsoft::WRL::ComPtr<ID3D12Resource> mBufferUploader = nullptr;
	std::unique_ptr<DX12Resource> mBufferDXResource = nullptr;

	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
};

DEFINE_RESOURCE_CAST(DX12Buffer, RHIBuffer)
}
