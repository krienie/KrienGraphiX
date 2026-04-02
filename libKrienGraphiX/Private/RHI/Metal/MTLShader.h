
#pragma once

#include <Metal/Metal.hpp>
#include <metal_irconverter_runtime.h>
#include <Metal/MTL4ArgumentTable.hpp>
#include <Metal/MTLBuffer.hpp>
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

	[[nodiscard]]
	MTL4::ArgumentTable* getArgumentTable() const;

	void setTopLevelBufferEntries(const std::vector<IRDescriptorTableEntry>& entries) const;

private:
	bool createArgumentTables(const CompiledShader& compiledShader);

	MTLVertexLayout mVertexLayout;
	ShaderType mShaderType = ShaderType::Unassigned;

	NS::SharedPtr<MTL::Library> mLibrary;
	NS::SharedPtr<MTL4::LibraryFunctionDescriptor> mLibraryFunctionDesc;
	std::vector<NS::SharedPtr<MTL::Buffer>> mTopLevelBuffers;
	std::vector<NS::SharedPtr<MTL4::ArgumentTable>> mArgumentTables;
};

DEFINE_RESOURCE_CAST(MTLShader, RHIShader);
}
