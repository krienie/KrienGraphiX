
#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ShaderCompiler/ShaderCompiler.h>

#include "Private/Rendering/KGXShaderParameter.h"
#include "Private/RHI/RHIConstantBuffer.h"
#include "Private/RHI/RHIShaderProgram.h"

namespace kgx::rendering
{
class KGXShaderProgram
{
    public:
        using InputElementFormat = kgx::VertexInputElementFormat;
        using VertexInputElement = kgx::VertexInputElement;

        KGXShaderProgram(std::string name);
        virtual ~KGXShaderProgram() = default;

        [[nodiscard]] std::string getName() const;

        void loadFromFile(const std::string& pathToShaderFile);

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
