
#pragma once

#include <memory>
#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTLVertexDescriptor.hpp>

#include "Private/RHI/RHIUtils.h"
#include "Private/RHI/RHIVertexLayout.h"

namespace kgx::RHI
{
class MTLVertexLayout : public RHIVertexLayout
{
public:
	MTLVertexLayout() = default;
	~MTLVertexLayout() override = default;

	[[nodiscard]]
	MTL::VertexDescriptor* getNativeVertexDescriptor() const;

	void setFromInputElementVector(const std::vector<VertexInputElement>& vertexInputLayout) override;

private:
	NS::SharedPtr<MTL::VertexDescriptor> mVertexDescriptor;
};

DEFINE_RESOURCE_CAST(MTLVertexLayout, RHIVertexLayout);
}
