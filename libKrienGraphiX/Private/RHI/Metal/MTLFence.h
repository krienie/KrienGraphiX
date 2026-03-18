
#pragma once

#include <memory>
#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTLEvent.hpp>

#include "Private/RHI/RHIFence.h"

namespace kgx::RHI
{
class MTLFence : public RHIFence
{
public:
	MTLFence();
	~MTLFence() override = default;

	void sync() override;
	void queueSignal(uint64_t value) override;
	void waitForValue(uint64_t value) override;
	[[nodiscard]] uint64_t getCurrentValue() const override;

private:
	NS::SharedPtr<MTL::SharedEvent> mEvent;
	uint64_t mFenceValue = 0;
};
}
