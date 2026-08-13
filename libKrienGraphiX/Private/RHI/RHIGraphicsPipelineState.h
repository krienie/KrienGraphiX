
#pragma once

#include <xxh3.h>

#include "RHIDefinitions.h"

namespace kgx::RHI
{
class RHIGraphicsPipelineState
{
public:
	RHIGraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc)
		: mGraphicsDescriptor(desc)
	{
	}

	virtual ~RHIGraphicsPipelineState() = default;

	[[nodiscard]] const RHIGraphicsPipelineStateDescriptor& getDescriptor() const { return mGraphicsDescriptor; }

	virtual bool create() = 0;

protected:
	RHIGraphicsPipelineStateDescriptor mGraphicsDescriptor;
};
}

namespace kgx
{
inline XXH64_hash_t hash(const RHI::RHIGraphicsPipelineState& graphicsPipelineState)
{
	return hash(graphicsPipelineState.getDescriptor());
}
}
