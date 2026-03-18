
#pragma once

#include <memory>
#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTL4CommandAllocator.hpp>

#include "Private/RHI/RHICommandAllocator.h"
#include "Private/RHI/RHIUtils.h"

namespace kgx::RHI
{
class MTLCommandAllocator : public RHICommandAllocator
{
public:
	MTLCommandAllocator();
	~MTLCommandAllocator() override = default;

	[[nodiscard]]
	MTL4::CommandAllocator* getNativeAllocator() const;

	void reset() override;
private:
	NS::SharedPtr<MTL4::CommandAllocator> mCommandAllocator;
};

DEFINE_RESOURCE_CAST(MTLCommandAllocator, RHICommandAllocator);
}
