
#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Private/RHI/RHIConstantBuffer.h"
#include "ShaderCompiler/ShaderProgramCompiler.h"

namespace kgx::Rendering
{
using ConstantBufferDescriptor = kgx::ConstantBufferDescriptor;

class KGXConstantBuffer final
{
    public:
        KGXConstantBuffer(const ConstantBufferDescriptor & descriptor);
        ~KGXConstantBuffer() = default;

        [[nodiscard]] std::string name() const;

    private:
        std::unique_ptr<RHI::RHIConstantBuffer> mRHIConstantBuffer;
        std::unordered_map<std::string, RHI::RHIBufferVariable> mVariables;
};
}
