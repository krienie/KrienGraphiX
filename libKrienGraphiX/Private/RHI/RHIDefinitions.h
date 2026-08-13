
#pragma once

#include <cstdint>
#include <string>

#include <xxh3.h>

#include "RHIShader.h"
#include "RHIPixelFormat.h"
#include "RHIResource.h"
#include "KrienGraphiX/Core/Logging.h"

namespace kgx::RHI
{
struct RHIHeapHandle
{
	uint32_t index = -1;
	uint32_t generation = 0;

	[[nodiscard]] bool isValid() const { return index > -1; }
};

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
	DepthClear = 0x1,
	StencilClear = 0x1 << 1,

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

namespace kgx
{
XXH64_hash_t hash(const RHI::RHIGraphicsPipelineStateDescriptor& psoDescriptor);
}
