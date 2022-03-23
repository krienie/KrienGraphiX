
#include "DX12MemoryUtils.h"

namespace kgx::RHI::DX12MemoryUtils
{
UINT64 alignToBytes(UINT64 inSize, UINT64 align)
{
    return ((inSize - 1u) / align + 1u) * align;
}

UINT64 alignTo256Bytes(UINT64 inSize)
{
    return alignToBytes(inSize, 256u);
}

UINT64 alignTo64KBytes(UINT64 inSize)
{
    return alignToBytes(inSize, 65536u);
}
}