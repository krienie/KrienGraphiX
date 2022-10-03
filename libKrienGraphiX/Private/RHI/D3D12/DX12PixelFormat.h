
#pragma once

#include "d3dx12.h"
#include "Private/RHI/RHIPixelFormat.h"

namespace kgx::RHI
{
DXGI_FORMAT toDxgiPixelFormat(RHIPixelFormat inFormat);
}