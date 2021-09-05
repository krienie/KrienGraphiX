
#pragma once

#include <string>
#include <vector>

namespace kgx
{
enum class VertexInputElementFormat
{
    R_FLOAT,
    RGB_FLOAT,
    RGBA_FLOAT,
    R_INT,
    RGB_INT,
    RGBA_INT,
    R_UINT,
    RGB_UINT,
    RGBA_UINT
};

struct VertexInputElement
{
    std::string semanticName;
    VertexInputElementFormat format;
};

struct ShaderProgramDescriptor
{
    std::string name;
    std::vector<VertexInputElement> inputLayout;
    std::string vertexShaderPath;
    std::string hullShaderPath;
    std::string domainShaderPath;
    std::string geometryShaderPath;
    std::string pixelShaderPath;
};

struct BufferVariableDescriptor
{
    std::string name;
    unsigned int offset;
    unsigned int size;
};

struct ConstantBufferDescriptor
{
    std::string name;
    unsigned int bufferRegister;
    unsigned int size;
    std::vector<BufferVariableDescriptor> variables;
};

struct CompiledShader
{
    std::vector<char> byteCode;
    std::vector<char> pdb;
    std::vector<int> boundConstantBuffers;
};

struct CompiledShaderProgram
{
    CompiledShader vertexShader;
    CompiledShader hullShader;
    CompiledShader domainShader;
    CompiledShader geometryShader;
    CompiledShader pixelShader;
    std::vector<ConstantBufferDescriptor> constantBuffers;
};

const VertexInputElement VertexPositionInput = { "POSITION", VertexInputElementFormat::RGB_FLOAT };
const VertexInputElement VertexTexCoordInput = { "TEXCOORD", VertexInputElementFormat::RGB_FLOAT };
const VertexInputElement VertexNormalInput   = { "NORMAL",   VertexInputElementFormat::RGB_FLOAT };
const VertexInputElement VertexTangentInput  = { "TANGENT",  VertexInputElementFormat::RGB_FLOAT };

class ShaderProgramCompiler final
{
public:
    [[nodiscard]]
    static CompiledShaderProgram compileProgram(const ShaderProgramDescriptor &desc);
};
}