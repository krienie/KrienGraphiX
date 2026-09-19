#pragma once

#include <memory>

#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTL4CommandQueue.hpp>
#include <Metal/MTLResidencySet.hpp>

#include "MTLFence.h"

namespace kgx::RHI
{
class MTLCommandQueue final
{
public:
	MTLCommandQueue(MTLPlatform& platform);

	[[nodiscard]]
	MTL4::CommandQueue* getNativeCommandQueue() const { return mCommandQueue.get(); }

	void addGlobalResidency(const MTL::Allocation* allocation);
	void removeGlobalResidency(const MTL::Allocation* allocation);

	bool create();
	void executeCommandBuffer(MTL4::CommandBuffer* commandBuffer, bool waitForCompletion);
	void waitForCompletion() const;

private:
	bool mResidencySetDirty = false;
	NS::SharedPtr<MTL4::CommandQueue> mCommandQueue;
	NS::SharedPtr<MTL::ResidencySet> mResidencySet;
	std::unique_ptr<MTLFence> mFence;

	MTLPlatform& mPlatform;
};
}
