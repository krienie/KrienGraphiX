
#include "RHIMemoryUtils.h"

namespace kgx::RHI::MemoryUtils
{
uint64_t alignToBytes(uint64_t inSize, uint64_t align)
{
	return (inSize + (align - 1)) & ~(align - 1);
}
}
