
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

#ifdef _WIN32
	constexpr bool includeDebugInfo = true;
#elif defined(__APPLE__)
	constexpr bool includeDebugInfo = false;
#endif

	kgx::CompiledShader vertexShader;
	bool success = kgx::ShaderCompiler::compileShader(vertexShaderPath.string(), "main", "vs_6_5", includeDebugInfo, vertexShader);
	ASSERT_TRUE(success);

	kgx::CompiledShader pixelShader;
	success = kgx::ShaderCompiler::compileShader(pixelShaderPath.string(), "main", "ps_6_5", includeDebugInfo, pixelShader);
	ASSERT_TRUE(success);
	
	auto checkShader = [&](const kgx::CompiledShader& shader)
	{
		EXPECT_TRUE(!shader.byteCode.empty());
		EXPECT_EQ(includeDebugInfo, !shader.pdb.empty());
		EXPECT_EQ(shader.mainEntry, "main");
	};
	
	checkShader(vertexShader);
	checkShader(pixelShader);
}
