
#pragma once

#include <ShaderCompiler/ShaderCompiler.h>

namespace kgx::RHI
{
class RHIGraphicsDevice;

class RHIShader
{
    public:
        enum class ShaderType
        {
            Vertex,
            Hull,
            Domain,
            Geometry,
            Pixel
            //TODO(KL): add compute shader support
            //Compute
        };

        virtual bool init(RHIGraphicsDevice* device, const CompiledShader& compiledShader, ShaderType type) = 0;

        virtual ~RHIShader() = default;

        //virtual bool compile(const kgx::ShaderProgramDescriptor &desc) = 0;
        virtual void setVertexInputLayout(const std::vector<VertexInputElement>& vertexInputLayout) = 0;
        virtual bool loadCompiledShader(const CompiledShader& shaderDesc, ShaderType type) = 0;
        virtual bool loadConstantBuffers(const std::vector<ConstantBufferDescriptor>& bufferDesc) = 0;
};
}
