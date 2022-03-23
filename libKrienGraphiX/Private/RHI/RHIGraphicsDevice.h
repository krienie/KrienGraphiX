
#pragma once

namespace kgx::RHI
{
class RenderHardwareInterface;

class RHIGraphicsDevice
{
    public:
        virtual ~RHIGraphicsDevice() = default;

        virtual bool init() = 0;
};
}