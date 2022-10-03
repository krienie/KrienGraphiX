
#pragma once

namespace kgx::RHI
{
enum class RHIPixelFormat
{
    Unknown = 0,
    R32G32B32A32_typeless,
    R32G32B32A32_float,
    R32G32B32A32_uint,
    R32G32B32A32_sint,
    R32G32B32_typeless,
    R32G32B32_float,
    R32G32B32_uint,
    R32G32B32_sint,
    R16G16B16A16_typeless,
    R16G16B16A16_float,
    R16G16B16A16_unorm,
    R16G16B16A16_uint,
    R16G16B16A16_snorm,
    R16G16B16A16_sint,
    R32G32_typeless,
    R32G32_float,
    R32G32_uint,
    R32G32_sint,
    R24G8_typeless,
    D24_unorm_S8_uint,
    R24_unorm_X8_typeless,
    X24_typeless_G8_uint,
    R10G10B10A2_typeless,
    R10G10B10A2_unorm,
    R10G10B10A2_uint,
    R11G11B10_float,
    R8G8B8A8_typeless,
    R8G8B8A8_unorm,
    R8G8B8A8_unorm_srgb,
    R8G8B8A8_uint,
    R8G8B8A8_snorm,
    R8G8B8A8_sint,
    R16G16_typeless,
    R16G16_float,
    R16G16_unorm,
    R16G16_uint,
    R16G16_snorm,
    R16G16_sint,
    R32_typeless,
    D32_float,
    R32_float,
    R32_uint,
    R32_sint
};
}