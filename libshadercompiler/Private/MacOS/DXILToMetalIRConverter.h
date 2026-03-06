
#pragma once

#include <vector>
#include <string>

namespace kgx::DXILToMetalIRConverter
{
#ifdef __APPLE__
std::vector<char> convertToMetalIR(const std::vector<char>& dxilByteCode, const std::string& entryPoint);
#endif
}

