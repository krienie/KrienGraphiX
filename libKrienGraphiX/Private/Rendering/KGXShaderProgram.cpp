
#include "Private/Rendering/KGXShaderProgram.h"

#include <ShaderCompiler/ShaderCompiler.h>

#include "Private/Core/RenderCore.h"

namespace kgx::rendering
{
KGXShaderProgram::KGXShaderProgram(std::string name)
    : mName(std::move(name))//, mRHIShaderProgram(core::RenderCore::get()->getRHI()->createShaderProgram())
{
}

std::string KGXShaderProgram::getName() const
{
    return mName;
}

void KGXShaderProgram::loadFromFile(const std::string& pathToShaderFile)
{
    //const kgx::CompiledShaderProgram csp = kgx::ShaderProgramCompiler::compileProgram(desc);
    kgx::CompiledShader compiledShader;
    bool success = kgx::ShaderCompiler::compileShader(pathToShaderFile, "TODO", true, compiledShader);

    using ShaderType = RHI::RHIShaderProgram::ShaderType;
    auto addToProgramIfValid = [this](const kgx::CompiledShader & compiledShader, ShaderType type)
    {
        if (!compiledShader.byteCode.empty())
        {
            mRHIShaderProgram->loadCompiledShader(compiledShader, type);

            //TODO(KL): add shader resource descriptors (buffers, textures)
        }
    };

    //addToProgramIfValid(csp.vertexShader, ShaderType::Vertex);
    //addToProgramIfValid(csp.hullShader, ShaderType::Hull);
    //addToProgramIfValid(csp.domainShader, ShaderType::Domain);
    //addToProgramIfValid(csp.geometryShader, ShaderType::Geometry);
    //addToProgramIfValid(csp.pixelShader, ShaderType::Pixel);
    //TODO(KL): add compute shader support

    //TODO(KL): handle compile errors from the ShaderProgramCompiler


}

//void KGXShaderProgram::loadFromFile(const std::string &compiledShaderProgram)
//{
//    //mRHIShaderProgram->loadFromFile(shaderFile, type);
//}

}
