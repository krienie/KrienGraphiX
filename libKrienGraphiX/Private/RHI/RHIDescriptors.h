
#pragma once

#include <string>

#include "RHIShader.h"
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

enum RHIPrimitiveTopologyType
{
    Undefined = 0,
    Point = 1,
    Line = 2,
    Triangle = 3,
    Patch = 4
};

struct RHIGraphicsPipelineStateDescriptor
{
    RHIShader* VS;
    RHIShader* PS;
    RHIShader* DS;
    RHIShader* HS;
    RHIShader* GS;
    //RHIShader* AS; // To be implemented later
    //RHIShader* MS; // To be implemented later
    //TODO(KL): Implement blendstate
    //RHIBlendState BlendState;
    //TODO(KL): Implement rasterizerstate
    //RHIRasterizerState RasterizerState;
    RHIPrimitiveTopologyType PrimitiveTopology;
    unsigned int NumRenderTargets;
    RHIPixelFormat RenderTargetFormats[8];
    //TODO(KL): Implement MSAA support
    //SampleDesc.Count
    //SampleDesc.Quality
    RHIPixelFormat DepthStencilFormat;
};
}
