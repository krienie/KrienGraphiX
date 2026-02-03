
#pragma once

#include "RHIDescriptors.h"

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

	const RHIGraphicsPipelineStateDescriptor& getDescriptor() const { return mGraphicsDescriptor; }

	virtual bool create() = 0;

protected:
	RHIGraphicsPipelineStateDescriptor mGraphicsDescriptor;
};
}
