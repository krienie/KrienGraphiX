
#pragma once

namespace kgx::RHI
{
class RHIGraphicsCommandList;

class RHICommandQueue
{
	public:
		virtual ~RHICommandQueue() = default;

		virtual bool create() = 0;

		virtual void executeCommandList(RHIGraphicsCommandList* commandList) = 0;
		virtual void flushQueue() = 0;
};
}