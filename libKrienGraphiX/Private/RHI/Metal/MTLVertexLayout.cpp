
#include "MTLVertexLayout.h"

#include <Metal/Metal.hpp>
#include <metal_irconverter_runtime.h>
#include <Metal/MTLVertexDescriptor.hpp>

#include "Private/RHI/RHIMemoryUtils.h"

namespace
{
MTL::VertexFormat toNSVertexFormat(const kgx::VertexInputElementFormat& kgxVertexFormat)
{
	switch (kgxVertexFormat)
	{
	case kgx::VertexInputElementFormat::FLOAT:
		return MTL::VertexFormatFloat;
	case kgx::VertexInputElementFormat::FLOAT2:
		return MTL::VertexFormatFloat2;
	case kgx::VertexInputElementFormat::FLOAT3:
		return MTL::VertexFormatFloat3;
	case kgx::VertexInputElementFormat::FLOAT4:
		return MTL::VertexFormatFloat4;
	case kgx::VertexInputElementFormat::INT:
		return MTL::VertexFormatInt;
	case kgx::VertexInputElementFormat::INT2:
		return MTL::VertexFormatInt2;
	case kgx::VertexInputElementFormat::INT3:
		return MTL::VertexFormatInt3;
	case kgx::VertexInputElementFormat::INT4:
		return MTL::VertexFormatInt4;
	case kgx::VertexInputElementFormat::UINT:
		return MTL::VertexFormatUInt;
	case kgx::VertexInputElementFormat::UINT2:
		return MTL::VertexFormatUInt2;
	case kgx::VertexInputElementFormat::UINT3:
		return MTL::VertexFormatUInt3;
	case kgx::VertexInputElementFormat::UINT4:
		return MTL::VertexFormatUInt4;
	}

	return MTL::VertexFormatInvalid;
}

NS::UInteger getNSVertexFormatSize(const MTL::VertexFormat& format)
{
	switch (format)
	{
	// Float types
	case MTL::VertexFormatFloat:
		return 4;
	case MTL::VertexFormatFloat2:
		return 8;
	case MTL::VertexFormatFloat3:
		return 12;
	case MTL::VertexFormatFloat4:
		return 16;

	// Half-float types (16-bit)
	case MTL::VertexFormatHalf:
		return 2;
	case MTL::VertexFormatHalf2:
		return 4;
	case MTL::VertexFormatHalf3:
		return 6;
	case MTL::VertexFormatHalf4:
		return 8;

	// Integer types (32-bit)
	case MTL::VertexFormatInt:
		return 4;
	case MTL::VertexFormatInt2:
		return 8;
	case MTL::VertexFormatInt3:
		return 12;
	case MTL::VertexFormatInt4:
		return 16;

	case MTL::VertexFormatUInt:
		return 4;
	case MTL::VertexFormatUInt2:
		return 8;
	case MTL::VertexFormatUInt3:
		return 12;
	case MTL::VertexFormatUInt4:
		return 16;

	// Short types (16-bit)
	case MTL::VertexFormatShort:
	case MTL::VertexFormatShortNormalized:
		return 2;
	case MTL::VertexFormatShort2:
	case MTL::VertexFormatShort2Normalized:
		return 4;
	case MTL::VertexFormatShort3:
	case MTL::VertexFormatShort3Normalized:
		return 6;
	case MTL::VertexFormatShort4:
	case MTL::VertexFormatShort4Normalized:
		return 8;

	case MTL::VertexFormatUShort:
	case MTL::VertexFormatUShortNormalized:
		return 2;
	case MTL::VertexFormatUShort2:
	case MTL::VertexFormatUShort2Normalized:
		return 4;
	case MTL::VertexFormatUShort3:
	case MTL::VertexFormatUShort3Normalized:
		return 6;
	case MTL::VertexFormatUShort4:
	case MTL::VertexFormatUShort4Normalized:
		return 8;

	// Byte types (8-bit)
	case MTL::VertexFormatUChar:
	case MTL::VertexFormatUCharNormalized:
		return 1;
	case MTL::VertexFormatUChar2:
	case MTL::VertexFormatUChar2Normalized:
		return 2;
	case MTL::VertexFormatUChar3:
	case MTL::VertexFormatUChar3Normalized:
		return 3;
	case MTL::VertexFormatUChar4:
	case MTL::VertexFormatUChar4Normalized:
		return 4;

		// Packed types (Metal 4 / 10-10-10-2)
	case MTL::VertexFormatInt1010102Normalized:
	case MTL::VertexFormatUInt1010102Normalized:
		return 4;

	default:
		// Handle specialized or invalid formats
		return 0;
	}
}
}

namespace kgx::RHI
{
MTL::VertexDescriptor* MTLVertexLayout::getNativeVertexDescriptor() const
{
	return mVertexDescriptor.get();
}

void MTLVertexLayout::setFromInputElementVector(const std::vector<VertexInputElement>& vertexInputLayout)
{
	mVertexDescriptor = NS::TransferPtr(MTL::VertexDescriptor::alloc()->init());

	uint64_t vertexStride = 0;
	for (int i = 0; i < vertexInputLayout.size(); ++i)
	{
		auto& kgxVertexLayout = vertexInputLayout[i];

		const MTL::VertexFormat nsVertexFormat = toNSVertexFormat(kgxVertexLayout.format);
		MTL::VertexAttributeDescriptor* attributeDesc = mVertexDescriptor->attributes()->object(kIRStageInAttributeStartIndex + i);
		attributeDesc->setFormat(nsVertexFormat);
		attributeDesc->setOffset(vertexStride);
		attributeDesc->setBufferIndex(kIRVertexBufferBindPoint);

		vertexStride += getNSVertexFormatSize(nsVertexFormat);
	}

	vertexStride = MemoryUtils::alignToBytes(vertexStride, 4);
	mVertexDescriptor->layouts()->object(kIRVertexBufferBindPoint)->setStride(vertexStride);
	mVertexDescriptor->layouts()->object(kIRVertexBufferBindPoint)->setStepRate(1);
	mVertexDescriptor->layouts()->object(kIRVertexBufferBindPoint)->setStepFunction(MTL::VertexStepFunctionPerVertex);
}
}
