
#include "MTLPixelFormat.h"

namespace kgx::RHI
{
MTL::PixelFormat toMTLPixelFormat(RHIPixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
	case RHIPixelFormat::R32G32B32A32_typeless:
		return MTL::PixelFormatRGBA32Float;
	case RHIPixelFormat::R32G32B32A32_float:
		return MTL::PixelFormatRGBA32Float;
	case RHIPixelFormat::R32G32B32A32_uint:
		return MTL::PixelFormatRGBA32Uint;
	case RHIPixelFormat::R32G32B32A32_sint:
		return MTL::PixelFormatRGBA32Sint;
	case RHIPixelFormat::R32G32B32_typeless:
		return MTL::PixelFormatRGBA32Float;
	case RHIPixelFormat::R32G32B32_float:
		return MTL::PixelFormatRGBA32Float;
	case RHIPixelFormat::R32G32B32_uint:
		return MTL::PixelFormatRGBA32Uint;
	case RHIPixelFormat::R32G32B32_sint:
		return MTL::PixelFormatRGBA32Sint;
	case RHIPixelFormat::R16G16B16A16_typeless:
		return MTL::PixelFormatRGBA16Float;
	case RHIPixelFormat::R16G16B16A16_float:
		return MTL::PixelFormatRGBA16Float;
	case RHIPixelFormat::R16G16B16A16_unorm:
		return MTL::PixelFormatRGBA16Unorm;
	case RHIPixelFormat::R16G16B16A16_uint:
		return MTL::PixelFormatRGBA16Uint;
	case RHIPixelFormat::R16G16B16A16_snorm:
		return MTL::PixelFormatRGBA16Snorm;
	case RHIPixelFormat::R16G16B16A16_sint:
		return MTL::PixelFormatRGBA16Sint;
	case RHIPixelFormat::R32G32_typeless:
		return MTL::PixelFormatRG32Float;
	case RHIPixelFormat::R32G32_float:
		return MTL::PixelFormatRG32Float;
	case RHIPixelFormat::R32G32_uint:
		return MTL::PixelFormatRG32Uint;
	case RHIPixelFormat::R32G32_sint:
		return MTL::PixelFormatRG32Sint;
	case RHIPixelFormat::R24G8_typeless:
		return MTL::PixelFormatDepth32Float_Stencil8;
	case RHIPixelFormat::D24_unorm_S8_uint:
		return MTL::PixelFormatDepth24Unorm_Stencil8;
	case RHIPixelFormat::R24_unorm_X8_typeless:
		return MTL::PixelFormatDepth32Float;
	case RHIPixelFormat::X24_typeless_G8_uint:
		return MTL::PixelFormatX24_Stencil8;
	case RHIPixelFormat::R10G10B10A2_typeless:
		return MTL::PixelFormatRGB10A2Unorm;
	case RHIPixelFormat::R10G10B10A2_unorm:
		return MTL::PixelFormatRGB10A2Unorm;
	case RHIPixelFormat::R10G10B10A2_uint:
		return MTL::PixelFormatRGB10A2Uint;
	case RHIPixelFormat::R11G11B10_float:
		return MTL::PixelFormatRG11B10Float;
	case RHIPixelFormat::R8G8B8A8_typeless:
		return MTL::PixelFormatRGBA8Unorm;
	case RHIPixelFormat::R8G8B8A8_unorm:
		return MTL::PixelFormatRGBA8Unorm;
	case RHIPixelFormat::R8G8B8A8_unorm_srgb:
		return MTL::PixelFormatRGBA8Unorm_sRGB;
	case RHIPixelFormat::R8G8B8A8_uint:
		return MTL::PixelFormatRGBA8Uint;
	case RHIPixelFormat::R8G8B8A8_snorm:
		return MTL::PixelFormatRGBA8Snorm;
	case RHIPixelFormat::R8G8B8A8_sint:
		return MTL::PixelFormatRGBA8Sint;
	case RHIPixelFormat::R16G16_typeless:
		return MTL::PixelFormatRG16Float;
	case RHIPixelFormat::R16G16_float:
		return MTL::PixelFormatRG16Float;
	case RHIPixelFormat::R16G16_unorm:
		return MTL::PixelFormatRG16Unorm;
	case RHIPixelFormat::R16G16_uint:
		return MTL::PixelFormatRG16Uint;
	case RHIPixelFormat::R16G16_snorm:
		return MTL::PixelFormatRG16Snorm;
	case RHIPixelFormat::R16G16_sint:
		return MTL::PixelFormatRG16Sint;
	case RHIPixelFormat::R32_typeless:
		return MTL::PixelFormatR32Float;
	case RHIPixelFormat::D32_float:
		return MTL::PixelFormatDepth32Float;
	case RHIPixelFormat::R32_float:
		return MTL::PixelFormatR32Float;
	case RHIPixelFormat::R32_uint:
		return MTL::PixelFormatR32Uint;
	case RHIPixelFormat::R32_sint:
		return MTL::PixelFormatR32Sint;
	default:
		return MTL::PixelFormatInvalid;
	}
}
}

