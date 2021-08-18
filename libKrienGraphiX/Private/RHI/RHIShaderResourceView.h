
#pragma once

namespace kgx::RHI
{
class RHIGraphicsDevice;

class RHIShaderResourceView
{
    public:
        virtual ~RHIShaderResourceView() = default;

        virtual bool init(RHIGraphicsDevice *device) = 0;
};
}
