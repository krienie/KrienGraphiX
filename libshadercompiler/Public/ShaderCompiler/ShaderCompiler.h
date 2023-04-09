
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

const VertexInputElement VertexPositionInput = { "POSITION", VertexInputElementFormat::FLOAT4 };
const VertexInputElement VertexTexCoordInput = { "TEXCOORD", VertexInputElementFormat::FLOAT2 };
const VertexInputElement VertexNormalInput   = { "NORMAL",   VertexInputElementFormat::FLOAT3 };
const VertexInputElement VertexTangentInput  = { "TANGENT",  VertexInputElementFormat::FLOAT3 };
const VertexInputElement VertexColorInput    = { "COLOR", VertexInputElementFormat::FLOAT4 };

struct BufferVariableDescriptor
{
    std::string name;
    unsigned int offset = 0;
    unsigned int size = 0;
};

struct ConstantBufferDescriptor
{
    std::string name;
    unsigned int bufferRegister = 0;
    unsigned int size = 0;
    std::vector<BufferVariableDescriptor> variables;
};

struct CompiledShader
{
    std::vector<char> byteCode;
    std::vector<char> pdb;
    std::vector<ConstantBufferDescriptor> constantBuffers;
};

class ShaderCompiler final
{
public:
    [[nodiscard]]
    static bool compileShader(const std::string& sourceFile, const std::string& target, bool includeDebugInfo, CompiledShader& OutCompiledShader);
};
}
