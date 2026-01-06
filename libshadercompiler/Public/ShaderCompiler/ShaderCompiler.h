
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

const VertexInputElement VertexPositionInput = { .semanticName = "POSITION", .format = VertexInputElementFormat::FLOAT3 };
const VertexInputElement VertexTexCoordInput = { .semanticName = "TEXCOORD", .format = VertexInputElementFormat::FLOAT2 };
const VertexInputElement VertexNormalInput   = { .semanticName = "NORMAL",   .format = VertexInputElementFormat::FLOAT3 };
const VertexInputElement VertexTangentInput  = { .semanticName = "TANGENT",  .format = VertexInputElementFormat::FLOAT3 };
const VertexInputElement VertexColorInput    = { .semanticName = "COLOR",    .format = VertexInputElementFormat::FLOAT4 };

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
	static bool compileShader(const std::string& sourceFile, const std::string& mainEntry, const std::string& target, bool includeDebugInfo, CompiledShader& OutCompiledShader);
};
}
