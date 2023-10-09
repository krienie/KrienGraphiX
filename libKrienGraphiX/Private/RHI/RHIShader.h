
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
            Pixel,
            //TODO(KL): add compute shader support
            //Compute

            Unassigned
        };

        RHIShader() = default;
        virtual ~RHIShader() = default;
        RHIShader(RHIShader& rhs) = default;
        RHIShader(RHIShader&& rhs) = default;
        RHIShader& operator=(const RHIShader& rhs) = default;
        RHIShader& operator=(RHIShader&& rhs) = default;

        virtual bool create(RHIGraphicsDevice* device, const CompiledShader& compiledShader, ShaderType type) = 0;
    
        virtual void setVertexInputLayout(const std::vector<VertexInputElement>& vertexInputLayout) = 0;

    protected:
        virtual bool loadConstantBuffers(const std::vector<ConstantBufferDescriptor>& bufferDesc) = 0;
};
}
