
#pragma once

#include <vector>
#include <memory>

#include "RHIDefinitions.h"
#include "RHITexture2D.h"
#include "RHITextureHandle.h"

namespace kgx::RHI
{
class RHITextureRegistry
{
public:
	[[nodiscard]] RHITextureHandle registerTexture(std::unique_ptr<RHITexture2D>&& texture);
	void releaseTexture(const RHITextureHandle& handle);

	[[nodiscard]] RHITexture2D* get(const RHITextureHandle& handle) const;
	[[nodiscard]] bool isValid(const RHITextureHandle& handle) const;

private:
	struct Slot
	{
		std::unique_ptr<RHITexture2D> texture;
		uint32_t generation = 0;
	};

	uint32_t findFreeSlot();

	std::vector<Slot> mPool;
	std::vector<uint32_t> mFreeIndices;
};
}
