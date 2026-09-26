
#pragma once

#include <memory>
#include <vector>

#include <Metal/Metal.hpp>
#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTLRenderPipeline.hpp>

#include "Private/RHI/RHIGraphicsPipelineState.h"
#include "Private/RHI/RHIUtils.h"

namespace kgx::RHI
{
class MTLGraphicsPipelineState final : public RHIGraphicsPipelineState
{
public:
	MTLGraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc);
	~MTLGraphicsPipelineState() override = default;

	bool create() override;

	[[nodiscard]]
	MTL::RenderPipelineState* getPSO() const;

private:
	NS::SharedPtr<MTL::RenderPipelineState> mRenderPipelineState;
};

DEFINE_RESOURCE_CAST(MTLGraphicsPipelineState, RHIGraphicsPipelineState);
}
