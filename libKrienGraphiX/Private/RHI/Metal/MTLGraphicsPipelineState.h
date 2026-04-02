
#pragma once

#include <memory>
#include <vector>

#include <Metal/Metal.hpp>
#include <metal_irconverter_runtime.h>
#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTL4ArgumentTable.hpp>
#include <Metal/MTLRenderPipeline.hpp>

#include "MTLShader.h"
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

	[[nodiscard]]
	MTL4::ArgumentTable* getArgumentTable() const;

	void setTopLevelBufferEntries(const std::vector<IRDescriptorTableEntry>& entries) const;

private:
	NS::SharedPtr<MTL::RenderPipelineState> mRenderPipelineState;
	// This is the shader that will be used for setting argumentbuffer and toplevelbuffer.
	// Will be changed to a different system later on.
	MTLShader* mMainShader = nullptr;
};

DEFINE_RESOURCE_CAST(MTLGraphicsPipelineState, RHIGraphicsPipelineState);
}
