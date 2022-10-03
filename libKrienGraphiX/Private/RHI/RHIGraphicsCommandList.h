
#pragma once

#include <cstdint>

#include "RHIResourceView.h"
#include "KrienGraphiX/Core/KGXDefinitions.h"

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

        virtual void reset(RHICommandQueue* commandQueue) = 0;

        virtual void setViewport(const KGXViewport& viewport) = 0;

        virtual void clearDepthStencilView(RHIResourceView* dsv, RHIResourceView::DepthStencilFlags clearFlags, float depth, uint8_t stencil) = 0;
        virtual void clearRenderTargetView(RHIResourceView* rtv, const float colorRGBA[4]) = 0;
        //TODO(KL): add other commandList methods
};
}
