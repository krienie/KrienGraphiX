
#pragma once

#include <cstdint>
#include <vector>

#include "RHIResourceView.h"
#include "Private/RHI/RHIDescriptors.h"
#include "Private/Core/PrivateDefinitions.h"

namespace kgx::core
{
class CommandListAllocator;
}

namespace kgx::RHI
{
class RHIResourceView;
class RHICommandQueue;
class RHIGraphicsPipelineState;

class RHIGraphicsCommandList
{
public:
	RHIGraphicsCommandList(core::CommandListAllocator& allocator);

	virtual ~RHIGraphicsCommandList() = default;

	virtual bool create(RHIGraphicsPipelineState* initialState) = 0;

	void release();

	virtual void close() = 0;

	virtual void reset(RHIGraphicsPipelineState* initialState = nullptr) = 0;

	virtual void setPipelineState(RHIGraphicsPipelineState* pipelineState) = 0;

	virtual void setViewport(const core::KGXViewport& viewport) = 0;

	virtual void setRenderTargets(const std::vector<RHIResourceView*>& renderTargetViews, const RHIResourceView* depthStencilView) = 0;

	virtual void clearDepthStencilView(RHIResourceView* dsv, DepthStencilFlags clearFlags, float depth, uint8_t stencil) = 0;
	virtual void clearRenderTargetView(RHIResourceView* rtv, const float colorRGBA[4]) = 0;
	//TODO(KL): add other commandList methods

private:
	core::CommandListAllocator* mAllocator;
};

class RHIGraphicsCommandListHandle
{
public:
	RHIGraphicsCommandListHandle(RHIGraphicsCommandList& commandList);
	~RHIGraphicsCommandListHandle();

	RHIGraphicsCommandList* get() const { return mCommandList; }

	RHIGraphicsCommandList* operator->() const { return mCommandList; }

private:
	RHIGraphicsCommandList* mCommandList;
};
}
