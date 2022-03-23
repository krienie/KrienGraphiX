
#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ShaderCompiler/ShaderProgramCompiler.h>

#include "Private/Rendering/KGXShaderParameter.h"
#include "Private/RHI/RHIConstantBuffer.h"
#include "Private/RHI/RHIShaderProgram.h"

namespace kgx::Rendering
{
class KGXShaderProgram
{
    public:
        using InputElementFormat = kgx::VertexInputElementFormat;
        using VertexInputElement = kgx::VertexInputElement;
        using ShaderProgramDescriptor = kgx::ShaderProgramDescriptor;

        KGXShaderProgram(std::string name);
        virtual ~KGXShaderProgram() = default;

        [[nodiscard]] std::string getName() const;

        void loadFromDescriptor(const kgx::ShaderProgramDescriptor & desc);

        //void loadFromFile(const std::string &compiledShaderProgram);

//    protected:
//        RHI::RHIShaderProgram * getShaderProgram() const;

    private:
        

        std::string mName;
        std::unique_ptr<RHI::RHIShaderProgram> mRHIShaderProgram;

        std::vector<RHI::RHIConstantBuffer> mRHIConstantBuffers;
        std::vector<KGXShaderParameter> mShaderParameters;
};
}
