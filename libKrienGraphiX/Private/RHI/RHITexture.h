
#pragma once

#include "RHIResource.h"
#include "RHIPixelFormat.h"
#include "RHIResource.h"

namespace kgx::RHI
{
class RHIGraphicsDevice;

class RHITexture : public RHIResource
{
    public:
        RHITexture(CreationFlags flags, unsigned int numMips, unsigned int numSamples, RHIPixelFormat pixelFormat)
            : RHIResource(flags), mNumMips(numMips), mNumSamples(numSamples), mPixelFormat(pixelFormat)
        {
            //TODO(KL): CreationFlags::None for now
        }

        ~RHITexture() override = default;
        
        [[nodiscard]]
        unsigned int numMips() const { return mNumMips; }

        [[nodiscard]]
        unsigned int numSamples() const { return mNumSamples; }

        [[nodiscard]]
        RHIPixelFormat pixelFormat() const { return mPixelFormat; }

    private:
        unsigned int mNumMips;
        unsigned int mNumSamples;
        RHIPixelFormat mPixelFormat;
};
}
