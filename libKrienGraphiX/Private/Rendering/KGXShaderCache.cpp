
#include "Private/Rendering/KGXShaderCache.h"

#include "Private/Core/CommandListAllocator.h"
#include "Private/RHI/RenderHardwareInterface.h"
#include "ShaderCompiler/ShaderCompiler.h"

namespace kgx::rendering
{
KGXShaderCache::KGXShaderCache(RHI::RHIGraphicsDevice* graphicsDevice, core::CommandListAllocator* commandListAllocator)
    : mGraphicsDevice(graphicsDevice), mCommandListAllocator(commandListAllocator)
{
}

bool KGXShaderCache::loadShaderFromFile(const std::string& shaderFilePath, const std::string& mainEntry, RHI::RHIShader::ShaderType type)
{
    if (mLoadedShaders.contains(shaderFilePath))
    {
        // Shader is already loaded. Do nothing.
        return true;
    }

    CompiledShader compiledShader;

    constexpr bool includeDebugInfo = true;
    const bool success = ShaderCompiler::compileShader(shaderFilePath, mainEntry, includeDebugInfo, compiledShader);

    if (!success)
    {
        return false;
    }

    std::unique_ptr<RHI::RHIShader> newShader = RHI::PlatformRHI->createShader(mGraphicsDevice, mCommandList, compiledShader, type);
    if (!newShader)
    {
        return false;
    }

    mLoadedShaders[shaderFilePath] = std::move(newShader);

    return true;
}

    //TODO(KL): Find a better way to retrieve shaders than to lookup their original file path every time..
RHI::RHIShader* KGXShaderCache::getLoadedShader(const std::string& originalShaderFilePath) const
{
    if (const auto foundShader = mLoadedShaders.find(originalShaderFilePath); foundShader != mLoadedShaders.end())
    {
        return foundShader->second.get();
    }

    return nullptr;
}
}
