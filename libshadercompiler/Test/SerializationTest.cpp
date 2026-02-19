
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
	bool success = kgx::ShaderCompiler::compileShader(vertexShaderPath.string(), "main", "vs_6_5", true, vertexShader);
	ASSERT_TRUE(success);

	kgx::CompiledShader pixelShader;
	success = kgx::ShaderCompiler::compileShader(pixelShaderPath.string(), "main", "ps_6_5", true, pixelShader);
	ASSERT_TRUE(success);
	
	auto checkShader = [](const kgx::CompiledShader& shader)
	{
		EXPECT_TRUE(!shader.byteCode.empty());
		EXPECT_TRUE(!shader.pdb.empty());
	};
	
	checkShader(vertexShader);
	checkShader(pixelShader);
}
