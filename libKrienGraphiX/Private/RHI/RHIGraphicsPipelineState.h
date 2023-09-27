
#pragma once

#include "RHIDescriptors.h"

namespace kgx::RHI
{
class RHIGraphicsPipelineState
{
public:
    RHIGraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc)
        : mGraphicsDescriptor(desc)
    {
    }

    virtual ~RHIGraphicsPipelineState() = default;

    virtual bool create(RHIGraphicsDevice* device) = 0;

protected:
    RHIGraphicsPipelineStateDescriptor mGraphicsDescriptor;
};
}
