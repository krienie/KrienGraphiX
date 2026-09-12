
#pragma once

#include <cstdint>

namespace kgx::RHI
{
struct RHITextureHandle
{
	uint32_t index = 0;
	uint32_t generation = 0;
};
}
