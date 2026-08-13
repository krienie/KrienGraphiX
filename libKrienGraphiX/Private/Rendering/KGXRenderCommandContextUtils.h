
#pragma once

#include <cstdint>

namespace kgx::rendering
{
struct TextureHandle
{
	int32_t index = -1;

	[[nodiscard]] bool isValid() const { return index > -1; }
};
}
