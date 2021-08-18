
#include "ShaderCompiler/ShaderProgramCompiler.h"

#include "ShaderCompiler.h"

#include <dxc/d3d12shader.h>
#include <dxc/dxcapi.h>
#include <wrl\client.h>

#include <iostream>

using namespace Microsoft::WRL;

namespace kgx
{
CompiledShaderProgram ShaderProgramCompiler::compileProgram(const ShaderProgramDescriptor & desc)
{
    std::cout << "Compiling shaders..." << std::endl;

    std::cout << "VertexShader: " << desc.vertexShaderPath << std::endl;
    std::cout << "PixelShader: " << desc.pixelShaderPath << std::endl;

    ShaderCompiler compiler;
    CompiledShaderProgram program;

    //TODO(KL): add some proper error handling

    if (!desc.vertexShaderPath.empty())
    {
        program.vertexShader = compiler.compileShader(desc.vertexShaderPath, "vs_6_5", true);
    }

    if (!desc.hullShaderPath.empty())
    {
        program.hullShader = compiler.compileShader(desc.hullShaderPath, "hs_6_5", true);
    }

    if (!desc.domainShaderPath.empty())
    {
        program.domainShader = compiler.compileShader(desc.domainShaderPath, "ds_6_5", true);
    }

    if (!desc.geometryShaderPath.empty())
    {
        program.geometryShader = compiler.compileShader(desc.geometryShaderPath, "gs_6_5", true);
    }

    if (!desc.pixelShaderPath.empty())
    {
        program.pixelShader = compiler.compileShader(desc.pixelShaderPath, "ps_6_5", true);
    }

    return program;
}
}
