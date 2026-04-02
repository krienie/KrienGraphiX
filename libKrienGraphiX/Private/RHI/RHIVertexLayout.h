
#pragma once

#include <ShaderCompiler/ShaderCompiler.h>

namespace kgx::RHI
{
class RHIVertexLayout
{
public:
	virtual ~RHIVertexLayout() = default;

	virtual void setFromInputElementVector(const std::vector<VertexInputElement>& vertexInputLayout) = 0;
};
}
