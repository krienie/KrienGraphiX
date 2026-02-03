
#pragma once

#include <ShaderCompiler/ShaderCompiler.h>

namespace kgx::RHI
{
class RHIBuffer;
class RHIGraphicsCommandList;

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

	virtual std::vector<const RHIBuffer*> getConstantBufferPtrs() const = 0;

protected:
	virtual bool loadConstantBuffers(const std::vector<ConstantBufferDescriptor>& bufferDesc) = 0;
};
}
