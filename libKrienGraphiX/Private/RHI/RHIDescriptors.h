
#pragma once

#include <string>

#include "RHIPixelFormat.h"
#include "RHIResource.h"

namespace kgx::RHI
{
struct RHIConstantBufferDescriptor
{
    std::string name;
    unsigned int bufferRegister;
    unsigned int bufferSize;
    RHIResource::CreationFlags flags = RHIResource::None;
};

struct RHIClearValue final
{
    struct DepthClear
    {
        float depth;
        uint8_t stencil;
    };

    union
    {
        float colorClear[4];
        DepthClear depthClear;
    };
};

struct RHITexture2DDescriptor
{
    RHIClearValue clearValue;
    RHIPixelFormat pixelFormat;
    unsigned int width;
    unsigned int height;
    unsigned int numMips = 1;
    unsigned int numSamples = 1;
    RHIResource::CreationFlags flags = RHIResource::None;
};
}
