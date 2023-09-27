
#pragma once

#include <cstdint>
#include <vector>

#include "RHIResourceView.h"
#include "Private/Core/PrivateDefinitions.h"

namespace kgx::RHI
{
class RHIResourceView;
class RHIGraphicsDevice;
class RHICommandQueue;
class RHIGraphicsPipelineState;

class RHIGraphicsCommandList
{
    public:
        virtual ~RHIGraphicsCommandList() = default;
        
        virtual bool init(RHIGraphicsDevice* device, RHICommandQueue* commandQueue, RHIGraphicsPipelineState* initialState) = 0;
        virtual void close() = 0;

        virtual void reset(RHICommandQueue* commandQueue, RHIGraphicsPipelineState* initialState = nullptr) = 0;

        virtual void setPipelineState(RHIGraphicsPipelineState* pipelineState) = 0;

        virtual void setViewport(const core::KGXViewport& viewport) = 0;

        virtual void setRenderTargets(const std::vector<RHIResourceView*>& renderTargetViews, const RHIResourceView* depthStencilView) = 0;

        virtual void clearDepthStencilView(RHIResourceView* dsv, RHIResourceView::DepthStencilFlags clearFlags, float depth, uint8_t stencil) = 0;
        virtual void clearRenderTargetView(RHIResourceView* rtv, const float colorRGBA[4]) = 0;
        //TODO(KL): add other commandList methods
};
}
