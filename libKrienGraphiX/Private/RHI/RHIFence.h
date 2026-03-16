
#pragma once

#include <cstdint>

namespace kgx::RHI
{
class RHIFence
{
public:
	virtual ~RHIFence() = default;

	virtual void sync() = 0;
	virtual void queueSignal(uint64_t value) = 0;
	virtual void waitForValue(uint64_t value) = 0;
	virtual uint64_t getCurrentValue() const = 0;
};
}