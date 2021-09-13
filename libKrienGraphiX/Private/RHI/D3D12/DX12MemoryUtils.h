
#pragma once

#include <d3d12.h>

namespace kgx::RHI::DX12MemoryUtils
{
UINT64 alignToBytes(UINT64 inSize, UINT64 align);
UINT64 alignTo256Bytes(UINT64 inSize);
UINT64 alignTo64KBytes(UINT64 inSize);
}
