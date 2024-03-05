
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
class RHIGraphicsDevice;
class RHICommandQueue;
class RHIGraphicsPipelineState;

class RHIGraphicsCommandList
{
public:
    RHIGraphicsCommandList(core::CommandListAllocator& allocator);

    virtual ~RHIGraphicsCommandList();
    
    virtual bool create(RHIGraphicsDevice* device, RHICommandQueue* commandQueue, RHIGraphicsPipelineState* initialState) = 0;

    void release();
    void close();

    [[nodiscard]]
    bool isClosed() const { return mIsClosed; }

    virtual void reset(RHICommandQueue* commandQueue, RHIGraphicsPipelineState* initialState = nullptr) = 0;

    virtual void setPipelineState(RHIGraphicsPipelineState* pipelineState) = 0;

    virtual void setViewport(const core::KGXViewport& viewport) = 0;

    virtual void setRenderTargets(const std::vector<RHIResourceView*>& renderTargetViews, const RHIResourceView* depthStencilView) = 0;

    virtual void clearDepthStencilView(RHIResourceView* dsv, DepthStencilFlags clearFlags, float depth, uint8_t stencil) = 0;
    virtual void clearRenderTargetView(RHIResourceView* rtv, const float colorRGBA[4]) = 0;
    //TODO(KL): add other commandList methods

private:
    virtual void closeInternal() = 0;

    bool mIsClosed = false;
    core::CommandListAllocator* mAllocator;
};
}
