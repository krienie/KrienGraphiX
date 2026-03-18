
#pragma once

#include <Metal/MTLPixelFormat.hpp>

#include "Private/RHI/RHIPixelFormat.h"

namespace kgx::RHI
{
MTL::PixelFormat toMTLPixelFormat(RHIPixelFormat pixelFormat);
}
