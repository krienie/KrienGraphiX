
#pragma once

#include <memory>

#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTL4CommandAllocator.hpp>

#include "Private/Core/ResourcePool.h"

namespace kgx::RHI
{
class MTLCommandAllocator final : public core::PooledType<MTLCommandAllocator>
{
public:
	MTLCommandAllocator(core::ResourcePool<MTLCommandAllocator>& parentPool);
	~MTLCommandAllocator() override = default;

	[[nodiscard]]
	MTL4::CommandAllocator* getNativeAllocator() const;

	void reset();

private:
	NS::SharedPtr<MTL4::CommandAllocator> mCommandAllocator;
};
}
