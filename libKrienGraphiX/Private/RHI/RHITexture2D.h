
#pragma once

#include "RHITexture.h"
#include "RHIDescriptors.h"

namespace kgx::RHI
{
class RHIGraphicsDevice;

class RHITexture2D : public RHITexture
{
    public:
        RHITexture2D(RHITexture2DDescriptor descriptor)
            : RHITexture(descriptor.flags, descriptor.numMips, descriptor.numSamples, descriptor.pixelFormat), mWidth(descriptor.width), mHeight(descriptor.height)
        {}

        ~RHITexture2D() override = default;

        [[nodiscard]]
        unsigned int width() const { return mWidth; }

        [[nodiscard]]
        unsigned int height() const { return mHeight; }

    private:
        unsigned int mWidth;
        unsigned int mHeight;
};
}
