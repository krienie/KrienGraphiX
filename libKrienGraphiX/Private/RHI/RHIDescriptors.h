
#pragma once

#include <string>

namespace kgx::RHI
{
struct RHIConstantBufferDescriptor
{
    std::string name;
    unsigned int bufferRegister;
    unsigned int bufferSize;
};
}
