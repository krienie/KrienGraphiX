#pragma once

#include <memory>

#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTL4CommandQueue.hpp>
#include <Metal/MTLResidencySet.hpp>

#include "Private/RHI/RHICommandQueue.h"
#include "Private/RHI/RHIResource.h"
#include "Private/RHI/RHIUtils.h"

namespace kgx::RHI
{
class MTLCommandQueue final : public RHICommandQueue
{
public:
	MTLCommandQueue();

	[[nodiscard]]
	MTL4::CommandQueue* getNativeCommandQueue() const { return mCommandQueue.get(); }

	void addGlobalResidency(const MTL::Allocation* allocation);

	bool create() override;
	void executeCommandList(RHIGraphicsCommandList* commandList) override;

private:
	bool mResidencySetDirty = false;
	NS::SharedPtr<MTL4::CommandQueue> mCommandQueue;
	NS::SharedPtr<MTL::ResidencySet> mResidencySet;
};

DEFINE_RESOURCE_CAST(MTLCommandQueue, RHICommandQueue);
}
