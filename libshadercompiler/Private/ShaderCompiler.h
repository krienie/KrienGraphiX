
#pragma once

#include "ShaderCompiler/ShaderProgramCompiler.h"

#include <dxc/d3d12shader.h>
#include <dxc/dxcapi.h>
#include <wrl\client.h>

#include <string>
#include <optional>

namespace kgx
{
class ShaderCompiler final
{
public:
    ShaderCompiler();

    [[nodiscard]]
    CompiledShader compileShader(const std::string & sourceFile, const std::string & target, bool includeDebugInfo);

private:
    Microsoft::WRL::ComPtr<IDxcUtils> mUtils;
    Microsoft::WRL::ComPtr<IDxcCompiler3> mCompiler;
};
}