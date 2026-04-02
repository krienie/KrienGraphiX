
#pragma once

#include <cstdint>

namespace kgx::RHI::MemoryUtils
{
uint64_t alignToBytes(uint64_t inSize, uint64_t align);

uint64_t constexpr alignTo256Bytes(uint64_t inSize)
{
	return (inSize + 255) & ~255;
}

uint64_t constexpr alignTo64KBytes(uint64_t inSize)
{
	return (inSize + 65535) & ~65535;
}
}
