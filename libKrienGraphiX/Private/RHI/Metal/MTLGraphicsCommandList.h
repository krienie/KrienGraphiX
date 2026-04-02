
#pragma once

#include <functional>
#include <queue>
#include <unordered_map>

#include <Metal/Metal.hpp>
#include <metal_irconverter_runtime.h>
#include <Metal/MTL4CommandAllocator.hpp>
#include <Metal/MTL4CommandBuffer.hpp>

#include "MTLTexture2D.h"

#include "Private/RHI/RHIGraphicsCommandList.h"
#include "Private/RHI/RHIUtils.h"

namespace kgx::RHI
{
struct MTLPassTextureDescriptor
{
	MTLTexture2D* texture = nullptr;
	MTL::LoadAction loadAction = MTL::LoadActionDontCare;
	MTL::StoreAction storeAction = MTL::StoreActionDontCare;
	RHIClearValue clearValue = {0, 0, 0, 0};
	DepthStencilFlags dsClearFlags;
};

struct MTLPassCompilationContext
{
	std::unordered_map<MTLTexture2D*, MTLPassTextureDescriptor> usedTextures;
	std::vector<MTLTexture2D*> renderTargets;
	MTLPassTextureDescriptor depthStencilTexture;
	std::vector<IRDescriptorTableEntry> topLevelBufferEntries;
	class MTLGraphicsPipelineState* mtlPipelineState = nullptr;
};

class MTLGraphicsCommandList final : public RHIGraphicsCommandList
{
public:
	MTLGraphicsCommandList();
	~MTLGraphicsCommandList() override = default;

	[[nodiscard]]
	MTL4::CommandBuffer* getCommandBuffer() const { return mCommandBuffer.get(); }

	bool create(RHIGraphicsPipelineState* initialState) override;

	void close() override;

	void reset(RHICommandAllocator* allocator, RHIGraphicsPipelineState* initialState) override;

	void setPipelineState(RHIGraphicsPipelineState* pipelineState) override;

	void setConstantBuffer(const RHIBuffer* constantBuffer) override;

	void setViewport(const core::KGXViewport &viewport) override;

	void setRenderTargets(const std::vector<RHIResourceView*>& renderTargetViews, const RHIResourceView* depthStencilView) override;

	void clearDepthStencilView(RHIResourceView* dsv, DepthStencilFlags clearFlags, float depth, uint8_t stencil) override;
	void clearRenderTargetView(RHIResourceView* rtv, const float colorRGBA[4]) override;

	void drawMeshRenderObject(const rendering::KGXMeshRenderObject* renderObject) override;

private:
	template <typename F>
	void recordEncoderCommand(F&& func)
	{
		mRecordedEncoderCommands.push(std::forward<F>(func));
	}

	[[nodiscard]]
	NS::SharedPtr<MTL4::RenderPassDescriptor> compileToRenderPassDescriptor() const;

	NS::SharedPtr<MTL4::CommandBuffer> mCommandBuffer = nullptr;
	MTL4::RenderCommandEncoder* mEncoder = nullptr;
	MTL4::RenderPassDescriptor* mPassDescriptor = nullptr;

	MTLPassCompilationContext mPassContext;
	std::queue<std::function<void(MTL4::RenderCommandEncoder*)>> mRecordedEncoderCommands;
};

DEFINE_RESOURCE_CAST(MTLGraphicsCommandList, RHIGraphicsCommandList);
}
