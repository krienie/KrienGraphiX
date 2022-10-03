
#pragma once
#include "RHIDescriptors.h"
#include "RHITexture2D.h"

namespace kgx::RHI
{
class RHIGraphicsDevice;

//TODO(KL): Is this class really needed?
class RHIDepthStencilBuffer : public RHITexture2D
{
public:
    RHIDepthStencilBuffer(RHITexture2DDescriptor descriptor)
        : RHITexture2D(descriptor) {}

    ~RHIDepthStencilBuffer() override = default;
};
}
