
#pragma once

#include "Private/RHI/RHIGraphicsCommandList.h"

#include <d3d12.h>
#include <wrl\client.h>

#include "DX12Utils.h"

namespace kgx::RHI
{
class DX12GraphicsCommandList final : public RHIGraphicsCommandList
{
public:
	DX12GraphicsCommandList(core::CommandListAllocator* allocator);
	~DX12GraphicsCommandList() override = default;

	[[nodiscard]]
	ID3D12GraphicsCommandList* getCommandList() const { return mCommandList.Get(); }

	bool create(RHIGraphicsPipelineState* initialState = nullptr) override;

	void close() override;

	void reset(RHIGraphicsPipelineState* initialState = nullptr) override;

	void setPipelineState(RHIGraphicsPipelineState* pipelineState) override;

	void setViewport(const core::KGXViewport &viewport) override;

	void setRenderTargets(const std::vector<RHIResourceView*>& renderTargetViews, const RHIResourceView* depthStencilView) override;

	void clearDepthStencilView(RHIResourceView* dsv, DepthStencilFlags clearFlags, float depth, uint8_t stencil) override;
	void clearRenderTargetView(RHIResourceView* rtv, const float colorRGBA[4]) override;

private:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> mCommandList;
};

DEFINE_RESOURCE_CAST(DX12GraphicsCommandList, RHIGraphicsCommandList)
}
