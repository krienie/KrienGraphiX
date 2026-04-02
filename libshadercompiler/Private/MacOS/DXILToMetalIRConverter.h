
#pragma once

#include <vector>
#include <string>

namespace kgx
{
struct CompiledShader;
}

namespace kgx::DXILToMetalIRConverter
{
#ifdef __APPLE__
bool convertToMetalIR(const std::vector<char>& dxilByteCode, const std::string& entryPoint, CompiledShader& outShader);
#endif
}

