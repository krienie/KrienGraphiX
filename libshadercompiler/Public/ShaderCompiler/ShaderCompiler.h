
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

enum class VertexInputClassification
{
	PerVertex   = 0,
	PerInstance = 1
};

struct VertexInputElement
{
	std::string semanticName;
	VertexInputElementFormat format;
	VertexInputClassification classification;
};

const VertexInputElement VertexPositionInput =
{
	.semanticName = "POSITION",
	.format = VertexInputElementFormat::FLOAT3,
	.classification = VertexInputClassification::PerVertex
};
const VertexInputElement VertexTexCoordInput =
{
	.semanticName = "TEXCOORD",
	.format = VertexInputElementFormat::FLOAT2,
	.classification = VertexInputClassification::PerVertex
};
const VertexInputElement VertexNormalInput =
{
	.semanticName = "NORMAL",
	.format = VertexInputElementFormat::FLOAT3,
	.classification = VertexInputClassification::PerVertex
};
const VertexInputElement VertexTangentInput =
{
	.semanticName = "TANGENT",
	.format = VertexInputElementFormat::FLOAT3,
	.classification = VertexInputClassification::PerVertex
};
const VertexInputElement VertexColorInput =
{
	.semanticName = "COLOR",
	.format = VertexInputElementFormat::FLOAT4,
	.classification = VertexInputClassification::PerVertex
};
const VertexInputElement VertexObjectIDInput =
{
	.semanticName = "OBJECT_ID",
	.format = VertexInputElementFormat::UINT,
	.classification = VertexInputClassification::PerInstance
};

struct BufferVariableDescriptor
{
	std::string name;
	unsigned int offset = 0;
	unsigned int size = 0;
};

struct ShaderReflection
{
	std::string mainEntry;
	unsigned int numResources = 0;
};

struct CompiledShader
{
	std::vector<char> byteCode;
	std::vector<char> pdb;
	ShaderReflection reflection;
};

class ShaderCompiler final
{
public:
	[[nodiscard]]
	static bool compileShader(const std::string& sourceFile, const std::string& mainEntry, const std::string& target, bool includeDebugInfo, CompiledShader& outCompiledShader);
};
}
