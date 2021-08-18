
#pragma once

#include "RHITexture.h"

namespace kgx::RHI
{
class RHIGraphicsDevice;

class RHITexture2D : public RHITexture
{
    public:
        RHITexture2D(unsigned int width, unsigned int height, unsigned int numMips, unsigned int numSamples, PixelFormat format)
            : RHITexture(numMips, numSamples, format), mWidth(width), mHeight(height)
        {}

        virtual ~RHITexture2D() = default;

        [[nodiscard]]
        unsigned int width() const { return mWidth; }

        [[nodiscard]]
        unsigned int height() const { return mHeight; }

    private:
        unsigned int mWidth;
        unsigned int mHeight;
};
}
