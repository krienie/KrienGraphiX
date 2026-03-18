
#pragma once

#include <string>

#include "RHIShader.h"
#include "RHIPixelFormat.h"
#include "RHIResource.h"

namespace kgx::RHI
{
struct RHIBufferDescriptor
{
	std::string name;
	unsigned int bufferSize;
	unsigned int bufferRegister;
	bool isBufferAligned;
	bool isDynamic;
	void* initialData;
	RHIResource::CreationFlags flags = RHIResource::None;
};

enum DepthStencilFlags
{
	DepthClear = 1,
	StencilClear = 2,

	DepthStencilClear = DepthClear | StencilClear
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
	RHIShader* vs;
	RHIShader* ps;
	RHIShader* ds;
	RHIShader* hs;
	RHIShader* gs;
	//RHIShader* as; // To be implemented later
	//RHIShader* ms; // To be implemented later
	//TODO(KL): Implement blendstate
	//RHIBlendState BlendState;
	//TODO(KL): Implement rasterizerstate
	//RHIRasterizerState RasterizerState;
	RHIPrimitiveTopologyType primitiveTopology;
	unsigned int numRenderTargets;
	RHIPixelFormat renderTargetFormats[8];
	//TODO(KL): Implement MSAA support
	//SampleDesc.Count
	//SampleDesc.Quality
	RHIPixelFormat depthStencilFormat;
};
}
