
#include "gtest/gtest.h"

#include <filesystem>
#include <string>

#include "ShaderCompiler/ShaderCompiler.h"

using namespace testing;


TEST(Serialization, ShaderCompilation)
{
	const auto vertexShaderPath = std::filesystem::absolute("./TestData/DefaultVS.hlsl");
	const auto pixelShaderPath = std::filesystem::absolute("./TestData/DefaultPS.hlsl");

	ASSERT_TRUE(std::filesystem::exists(vertexShaderPath));
	ASSERT_TRUE(std::filesystem::exists(pixelShaderPath));
	
	kgx::CompiledShader vertexShader;
	bool success = kgx::ShaderCompiler::compileShader(vertexShaderPath.string(), "vs_6_5", true, vertexShader);
	ASSERT_TRUE(success);

	kgx::CompiledShader pixelShader;
	success = kgx::ShaderCompiler::compileShader(pixelShaderPath.string(), "ps_6_5", true, pixelShader);
	ASSERT_TRUE(success);
	
	auto checkShader = [](const kgx::CompiledShader& shader)
	{
		EXPECT_TRUE(!shader.byteCode.empty());
		EXPECT_TRUE(!shader.pdb.empty());
	};
	
	checkShader(vertexShader);
	checkShader(pixelShader);

	ASSERT_EQ(vertexShader.constantBuffers.size(), 0u);
	ASSERT_EQ(pixelShader.constantBuffers.size(), 1u);

	const auto & constBuff = pixelShader.constantBuffers[0];
	EXPECT_EQ(constBuff.name, "pixelConstants");
	EXPECT_EQ(constBuff.bufferRegister, 0u);
	EXPECT_EQ(constBuff.size, 48u);

	const std::vector<kgx::BufferVariableDescriptor> expectedVars = {
		{.name = "float3Param", .offset = 0u, .size = 12u},
		{.name = "floatParam", .offset = 12u, .size = 4u},
		{.name = "intParam", .offset = 16u, .size = 4u},
		{.name = "int3Param", .offset = 20u, .size = 12u},
		{.name = "float4Param", .offset = 32u, .size = 16u}
	};

	ASSERT_EQ(constBuff.variables.size(), expectedVars.size());

	for (auto i = 0u; i < constBuff.variables.size(); ++i)
	{
		EXPECT_EQ(constBuff.variables[i].name, expectedVars[i].name);
		EXPECT_EQ(constBuff.variables[i].offset, expectedVars[i].offset);
		EXPECT_EQ(constBuff.variables[i].size, expectedVars[i].size);
	}
}
