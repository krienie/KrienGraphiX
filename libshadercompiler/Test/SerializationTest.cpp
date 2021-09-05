
#include "gtest/gtest.h"

#include <filesystem>
#include <string>

#include "ShaderCompiler/ShaderProgramCompiler.h"

using namespace testing;


TEST(Serialization, ShaderCompilation)
{
    const auto vertexShaderPath = std::filesystem::absolute("./TestData/DefaultVS.hlsl");
    const auto pixelShaderPath = std::filesystem::absolute("./TestData/DefaultPS.hlsl");

    ASSERT_TRUE(std::filesystem::exists(vertexShaderPath));
    ASSERT_TRUE(std::filesystem::exists(pixelShaderPath));
    
    kgx::ShaderProgramDescriptor spDesc;
    spDesc.name = "TestShaderProgram";
    spDesc.vertexShaderPath = vertexShaderPath.string();
    spDesc.pixelShaderPath = pixelShaderPath.string();

    const auto compiledShaderProgram = kgx::ShaderProgramCompiler::compileProgram(spDesc);
    
    auto checkShader = [](const kgx::CompiledShader & shader, bool expectToExist, auto expectedBoundBuffers)
    {
        if (expectToExist)
        {
            EXPECT_TRUE(!shader.byteCode.empty());
            EXPECT_TRUE(!shader.pdb.empty());

            ASSERT_EQ(shader.boundConstantBuffers.size(), expectedBoundBuffers.size());

            for (auto i = 0u; i < expectedBoundBuffers.size(); ++i)
            {
                EXPECT_EQ(shader.boundConstantBuffers[i], expectedBoundBuffers[i]);
            }
        }
        else
        {
            EXPECT_FALSE(!shader.byteCode.empty());
            EXPECT_FALSE(!shader.pdb.empty());
            EXPECT_TRUE(shader.boundConstantBuffers.empty());
        }
    };

    std::vector<int> emptyBoundBuffers;
    checkShader(compiledShaderProgram.vertexShader, true, emptyBoundBuffers);
    checkShader(compiledShaderProgram.hullShader, false, emptyBoundBuffers);
    checkShader(compiledShaderProgram.domainShader, false, emptyBoundBuffers);
    checkShader(compiledShaderProgram.geometryShader, false, emptyBoundBuffers);
    checkShader(compiledShaderProgram.pixelShader, true, std::vector<int>{0});

    ASSERT_EQ(compiledShaderProgram.constantBuffers.size(), 1u);

    const auto & constBuff = compiledShaderProgram.constantBuffers[0];
    EXPECT_EQ(constBuff.name, "pixelConstants");
    EXPECT_EQ(constBuff.bufferRegister, 0u);
    EXPECT_EQ(constBuff.size, 48u);

    const std::vector<kgx::BufferVariableDescriptor> expectedVars = {
        {"float3Param", 0u, 12u},
        {"floatParam", 12u, 4u},
        {"intParam", 16u, 4u},
        {"int3Param", 20u, 12u},
        {"float4Param", 32u, 16u}
    };

    ASSERT_EQ(constBuff.variables.size(), expectedVars.size());

    for (auto i = 0u; i < constBuff.variables.size(); ++i)
    {
        EXPECT_EQ(constBuff.variables[i].name, expectedVars[i].name);
        EXPECT_EQ(constBuff.variables[i].offset, expectedVars[i].offset);
        EXPECT_EQ(constBuff.variables[i].size, expectedVars[i].size);
    }
}
