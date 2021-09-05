
#pragma once

#include "ShaderCompiler/ShaderProgramCompiler.h"

#include <dxc/d3d12shader.h>
#include <dxc/dxcapi.h>
#include <wrl\client.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace kgx
{
class ShaderCompiler final
{
public:
    ShaderCompiler();

    [[nodiscard]]
    CompiledShader compileShader(const std::string & sourceFile, const std::string & target, bool includeDebugInfo);

    std::vector<ConstantBufferDescriptor> getCBufferDescriptors() const { return mConstantBufferDescriptors; }

private:
    Microsoft::WRL::ComPtr<IDxcUtils> mUtils;
    Microsoft::WRL::ComPtr<IDxcCompiler3> mCompiler;
    std::unordered_map<std::string, int> mProcessedCBuffers;
    std::vector<ConstantBufferDescriptor> mConstantBufferDescriptors;
};
}
