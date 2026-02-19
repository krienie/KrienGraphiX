
#pragma once

#include "Private/RHI/RHIGraphicsPipelineState.h"

#include <d3d12.h>
#include <wrl\client.h>

#include "DX12Utils.h"

namespace kgx::RHI
{
class DX12GraphicsPipelineState final : public RHIGraphicsPipelineState
{
public:
	DX12GraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc);
	~DX12GraphicsPipelineState() override = default;

	bool create() override;

	[[nodiscard]]
	ID3D12PipelineState* getPSO() const;

	[[nodiscard]]
	ID3D12RootSignature* getRootSignature() const;
	
private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
	//TODO(KL): Temporary here.
	ID3D12RootSignature* mRootSignature = nullptr;
};

DEFINE_RESOURCE_CAST(DX12GraphicsPipelineState, RHIGraphicsPipelineState)
}
