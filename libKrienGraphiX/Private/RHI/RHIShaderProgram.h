
#pragma once

#include <ShaderCompiler/ShaderProgramCompiler.h>

namespace kgx::RHI
{
class RHIGraphicsDevice;

class RHIShaderProgram
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

        virtual bool init(RHIGraphicsDevice *device) = 0;

        virtual ~RHIShaderProgram() = default;

        //virtual bool compile(const kgx::ShaderProgramDescriptor &desc) = 0;
        virtual bool setVertexInputLayout(/*define*/) = 0;
        virtual bool loadCompiledShader(const CompiledShader& shaderDesc, ShaderType type) = 0;
        virtual bool loadConstantBuffers(const std::vector<ConstantBufferDescriptor>& bufferDesc) = 0;
};
}
