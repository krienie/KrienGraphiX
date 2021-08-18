
#pragma once

#include "Private/RHI/PixelFormat.h"

namespace kgx::RHI
{
class RHIGraphicsDevice;

class RHITexture
{
    public:
        RHITexture(unsigned int numMips, unsigned int numSamples, PixelFormat format)
            : mNumMips(numMips), mNumSamples(numSamples), mFormat(format)
        {}

        virtual ~RHITexture() = default;
        
        [[nodiscard]]
        unsigned int numMips() const { return mNumMips; }

        [[nodiscard]]
        unsigned int numSamples() const { return mNumSamples; }

        [[nodiscard]]
        PixelFormat format() const { return mFormat; }

    private:
        unsigned int mNumMips;
        unsigned int mNumSamples;
        PixelFormat mFormat;
};
}
