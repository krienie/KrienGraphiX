
#pragma once

#include "Private/RHI/RHICommandAllocator.h"

#include <d3d12.h>
#include <wrl\client.h>

#include "Private/RHI/RHIUtils.h"

namespace kgx::RHI
{
class DX12CommandAllocator final : public RHICommandAllocator
{
public:
	DX12CommandAllocator();
	~DX12CommandAllocator() override = default;

	[[nodiscard]]
	ID3D12CommandAllocator* getNativeAllocator() const;

	void reset() override;

private:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDXCommandAllocator = nullptr;
};

DEFINE_RESOURCE_CAST(DX12CommandAllocator, RHICommandAllocator)
}
