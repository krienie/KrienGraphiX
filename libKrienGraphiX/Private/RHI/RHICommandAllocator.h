
#pragma once

namespace kgx::RHI
{
class RHICommandAllocator
{
public:
	virtual ~RHICommandAllocator() = default;

	virtual void reset() = 0;

	void release();
};
}