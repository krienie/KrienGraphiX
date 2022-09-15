
#pragma once

#include "KrienGraphiX/Core/KGXDefinitions.h"

namespace kgx::RHI
{
class RHIDepthStencilBuffer;
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

        virtual void clearDepthStencilView(RHIDepthStencilBuffer* dsv/*, clearFlags, depth, stencil*/) = 0;
        virtual void clearRenderTargetView(/*RHIRenderTargetView* rtv, const float colorRGBA[4]*/) = 0;
        //TODO(KL): add other commandList methods

        //TODO(KL): implement ResidencySets for D3D12
};
}
