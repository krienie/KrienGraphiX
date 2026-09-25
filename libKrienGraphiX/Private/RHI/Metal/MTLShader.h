
#pragma once

#include <Metal/Metal.hpp>
#include <metal_irconverter_runtime.h>
#include <Metal/MTL4LibraryFunctionDescriptor.hpp>
#include <Metal/MTLLibrary.hpp>

#include "MTLVertexLayout.h"
#include "Private/RHI/RHIShader.h"
#include "Private/RHI/RHIUtils.h"

namespace kgx::RHI
{
class MTLShader : public RHIShader
{
public:
	MTLShader() = default;
	~MTLShader() override = default;

	bool create(const CompiledShader& compiledShader, ShaderType type) override;
	void setVertexInputLayout(const std::vector<VertexInputElement>& vertexInputLayout) override;

	[[nodiscard]]
	MTL::VertexDescriptor* getMTLVertexDescriptor() const;

	[[nodiscard]]
	MTL4::LibraryFunctionDescriptor* getLibraryFunctionDescriptor() const;

private:
	MTLVertexLayout mVertexLayout;
	ShaderType mShaderType = ShaderType::Unassigned;

	NS::SharedPtr<MTL::Library> mLibrary;
	NS::SharedPtr<MTL4::LibraryFunctionDescriptor> mLibraryFunctionDesc;
};

DEFINE_RESOURCE_CAST(MTLShader, RHIShader);
}
