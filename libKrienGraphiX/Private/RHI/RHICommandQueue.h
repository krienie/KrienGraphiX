
#pragma once

#include "RHIFence.h"

namespace kgx::RHI
{
class RHIGraphicsCommandList;

class RHICommandQueue
{
public:
	RHICommandQueue();
	virtual ~RHICommandQueue() = default;

	virtual bool create() = 0;

	virtual void executeCommandList(RHIGraphicsCommandList* commandList) = 0;
	void flushQueue();

private:
	std::unique_ptr<RHIFence> mFence;
};
}
