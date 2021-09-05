
#pragma once

#include <string>
#include <vector>

namespace kgx
{
enum class VertexInputElementFormat
{
    FLOAT,
    FLOAT2,
    FLOAT3,
    FLOAT4,
    INT,
    INT2,
    INT3,
    INT4,
    UINT,
    UINT2,
    UINT3,
    UINT4
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

const VertexInputElement VertexPositionInput = { "POSITION", VertexInputElementFormat::FLOAT4 };
const VertexInputElement VertexTexCoordInput = { "TEXCOORD", VertexInputElementFormat::FLOAT2 };
const VertexInputElement VertexNormalInput   = { "NORMAL",   VertexInputElementFormat::FLOAT3 };
const VertexInputElement VertexTangentInput  = { "TANGENT",  VertexInputElementFormat::FLOAT3 };

class ShaderProgramCompiler final
{
public:
    [[nodiscard]]
    static CompiledShaderProgram compileProgram(const ShaderProgramDescriptor &desc);

private:
    ShaderProgramCompiler() = default;
};
}