
#pragma once

#include <ShaderCompiler/ShaderCompiler.h>

namespace kgx::RHI
{
class RHIBuffer;

class RHIShader
{
public:
	enum class ShaderType
	{
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		//TODO(KL): add compute shader support
		//Compute

		Unassigned
	};

	RHIShader() = default;
	virtual ~RHIShader() = default;

	virtual bool create(const CompiledShader& compiledShader, ShaderType type) = 0;

	virtual void setVertexInputLayout(const std::vector<VertexInputElement>& vertexInputLayout) = 0;
};
}
