
#pragma once

#include <cstdint>
#include <vector>

#include "RHIResourceView.h"
#include "Private/RHI/RHIDescriptors.h"
#include "Private/Core/PrivateDefinitions.h"

namespace kgx::rendering
{
class KGXMeshRenderObject;
}

namespace kgx::RHI
{
class RHIResourceView;
class RHICommandAllocator;
class RHICommandQueue;
class RHIGraphicsPipelineState;

class RHIGraphicsCommandList
{
public:
	virtual ~RHIGraphicsCommandList() = default;

	virtual bool create(RHIGraphicsPipelineState* initialState) = 0;

	void release();

	virtual void close() = 0;

	virtual void reset(RHICommandAllocator* allocator, RHIGraphicsPipelineState* initialState) = 0;

	virtual void setPipelineState(RHIGraphicsPipelineState* pipelineState) = 0;

	virtual void setConstantBuffer(const RHIBuffer* constantBuffer) = 0;

	virtual void setViewport(const core::KGXViewport& viewport) = 0;

	virtual void setRenderTargets(const std::vector<RHIResourceView*>& renderTargetViews, const RHIResourceView* depthStencilView) = 0;

	virtual void clearDepthStencilView(RHIResourceView* dsv, DepthStencilFlags clearFlags, float depth, uint8_t stencil) = 0;
	virtual void clearRenderTargetView(RHIResourceView* rtv, const float colorRGBA[4]) = 0;

	virtual void drawMeshRenderObject(const rendering::KGXMeshRenderObject* renderObject) = 0;
};
}
