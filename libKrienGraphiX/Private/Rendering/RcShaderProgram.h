
#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ShaderCompiler/ShaderProgramCompiler.h>

#include "Private/Rendering/RcShaderParameter.h"
#include "Private/RHI/RHIConstantBuffer.h"
#include "Private/RHI/RHIShaderProgram.h"

namespace kgx::Rendering
{
class RcShaderProgram
{
    public:
        using InputElementFormat = kgx::VertexInputElementFormat;
        using VertexInputElement = kgx::VertexInputElement;
        using ShaderProgramDescriptor = kgx::ShaderProgramDescriptor;

        RcShaderProgram(std::string name);
        virtual ~RcShaderProgram() = default;

        [[nodiscard]] std::string getName() const;

        void loadFromDescriptor(const kgx::ShaderProgramDescriptor & desc);

        //void loadFromFile(const std::string &compiledShaderProgram);

//    protected:
//        RHI::RHIShaderProgram * getShaderProgram() const;

    private:
        

        std::string mName;
        std::unique_ptr<RHI::RHIShaderProgram> mRHIShaderProgram;

        std::vector<RHI::RHIConstantBuffer> mRHIConstantBuffers;
        std::vector<RcShaderParameter> mShaderParameters;
};
}
