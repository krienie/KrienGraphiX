
#pragma once

#include "RHIResource.h"
#include "RHIPixelFormat.h"
#include "RHIViewableResource.h"

namespace kgx::RHI
{
class RHITexture : public RHIViewableResource
{
public:
	~RHITexture() override = default;

	[[nodiscard]]
	unsigned int numMips() const { return mNumMips; }

	[[nodiscard]]
	unsigned int numSamples() const { return mNumSamples; }

	[[nodiscard]]
	RHIPixelFormat pixelFormat() const { return mPixelFormat; }

	[[nodiscard]]
	bool IsTexture() const override { return true; }

protected:
	RHITexture(CreationFlags flags, unsigned int numMips, unsigned int numSamples, RHIPixelFormat pixelFormat)
	: RHIViewableResource(flags), mNumMips(numMips), mNumSamples(numSamples), mPixelFormat(pixelFormat)
	{}

private:
	unsigned int mNumMips;
	unsigned int mNumSamples;
	RHIPixelFormat mPixelFormat;
};
}
