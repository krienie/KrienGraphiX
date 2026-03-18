#pragma once

#include <memory>

#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTL4CommandQueue.hpp>

#include "Metal/MTLEvent.hpp"
#include "Private/RHI/RHICommandQueue.h"
#include "Private/RHI/RHIUtils.h"

namespace kgx::RHI
{
class MTLCommandQueue final : public RHICommandQueue
{
public:
	MTLCommandQueue();

	[[nodiscard]]
	MTL4::CommandQueue* getNativeCommandQueue() const { return mCommandQueue.get(); }

	bool create() override;
	void executeCommandList(RHIGraphicsCommandList* commandList) override;

private:
	NS::SharedPtr<MTL4::CommandQueue> mCommandQueue;
};

DEFINE_RESOURCE_CAST(MTLCommandQueue, RHICommandQueue);
}
