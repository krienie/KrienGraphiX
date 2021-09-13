
#include "Private/Rendering/RcShaderProgram.h"

#include <ShaderCompiler/ShaderProgramCompiler.h>

#include "Private/Core/RenderCore.h"

namespace kgx::Rendering
{
RcShaderProgram::RcShaderProgram(std::string name)
    : mName(std::move(name)), mRHIShaderProgram(core::RenderCore::get()->getRHI()->createShaderProgram())
{
}

std::string RcShaderProgram::getName() const
{
    return mName;
}

void RcShaderProgram::loadFromDescriptor(const kgx::ShaderProgramDescriptor & desc)
{
    const kgx::CompiledShaderProgram csp = kgx::ShaderProgramCompiler::compileProgram(desc);

    using ShaderType = RHI::RHIShaderProgram::ShaderType;
    auto addToProgramIfValid = [this](const kgx::CompiledShader & compiledShader, ShaderType type)
    {
        if (!compiledShader.byteCode.empty())
        {
            mRHIShaderProgram->loadCompiledShader(compiledShader.byteCode, compiledShader.boundConstantBuffers, type);

            //TODO(KL): add shader resource descriptors (buffers, textures)
        }
    };

    addToProgramIfValid(csp.vertexShader, ShaderType::Vertex);
    addToProgramIfValid(csp.hullShader, ShaderType::Hull);
    addToProgramIfValid(csp.domainShader, ShaderType::Domain);
    addToProgramIfValid(csp.geometryShader, ShaderType::Geometry);
    addToProgramIfValid(csp.pixelShader, ShaderType::Pixel);
    //TODO(KL): add compute shader support

    //TODO(KL): handle compile errors from the ShaderProgramCompiler


}

//void RcShaderProgram::loadFromFile(const std::string &compiledShaderProgram)
//{
//    //mRHIShaderProgram->loadFromFile(shaderFile, type);
//}

}
