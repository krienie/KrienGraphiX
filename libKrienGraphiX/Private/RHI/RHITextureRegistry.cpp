
#include "RHITextureRegistry.h"

namespace kgx::RHI
{
RHITextureHandle RHITextureRegistry::registerTexture(std::unique_ptr<RHITexture2D>&& texture)
{
	const uint32_t freeIndex = findFreeSlot();
	mPool[freeIndex].texture = std::forward<std::unique_ptr<RHITexture2D>>(texture);
	mPool[freeIndex].generation++;

	return { .index = freeIndex, .generation = mPool[freeIndex].generation };
}

void RHITextureRegistry::releaseTexture(const RHITextureHandle& handle)
{
	if (isValid(handle))
	{
		mPool[handle.index].texture.reset();
		mPool[handle.index].generation++;
		mFreeIndices.push_back(handle.index);
	}
}

RHITexture2D* RHITextureRegistry::get(const RHITextureHandle& handle) const
{
	if (isValid(handle))
	{
		return mPool[handle.index].texture.get();
	}

	return nullptr;
}

bool RHITextureRegistry::isValid(const RHITextureHandle& handle) const
{
	return handle.index < mPool.size() &&
		mPool[handle.index].texture != nullptr &&
		mPool[handle.index].generation == handle.generation;
}

uint32_t RHITextureRegistry::findFreeSlot()
{
	if (!mFreeIndices.empty())
	{
		const uint32_t index = mFreeIndices.back();
		mFreeIndices.pop_back();
		return index;
	}

	const uint32_t index = static_cast<uint32_t>(mPool.size());
	mPool.emplace_back();

	return index;
}
}
