
#pragma once

namespace kgx::RHI
{
class RHIGraphicsDevice;

class RHIDepthStencilBuffer
{
    public:
        virtual ~RHIDepthStencilBuffer() = default;

        virtual bool init(RHIGraphicsDevice *device) = 0;
};
}