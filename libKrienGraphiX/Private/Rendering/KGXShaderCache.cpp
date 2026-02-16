
#include "Private/Rendering/KGXShaderCache.h"

#include <cassert>

#include "Private/RHI/RenderHardwareInterface.h"
#include "ShaderCompiler/ShaderCompiler.h"

namespace
{
std::string shaderTypeToTargetString(kgx::RHI::RHIShader::ShaderType type)
{
	using namespace kgx::RHI;

	std::string shaderTypeStr;

	switch (type)
	{
	case RHIShader::ShaderType::Vertex:
		shaderTypeStr = "vs";
		break;
	case RHIShader::ShaderType::Hull:
		shaderTypeStr = "hs";
		break;
	case RHIShader::ShaderType::Domain:
		shaderTypeStr = "ds";
		break;
	case RHIShader::ShaderType::Geometry:
		shaderTypeStr = "gs";
		break;
	case RHIShader::ShaderType::Pixel:
		shaderTypeStr = "ps";
		break;
	case RHIShader::ShaderType::Unassigned:
		assert(false);
	}

	return shaderTypeStr + "_6_5";
}
}

namespace kgx::rendering
{
RHI::RHIShader* KGXShaderCache::loadShaderFromFile(const std::string& shaderFilePath, const std::string& mainEntry, RHI::RHIShader::ShaderType type)
{
	if (mLoadedShaders.contains(shaderFilePath))
	{
		return mLoadedShaders[shaderFilePath].get();
	}

	CompiledShader compiledShader;

	constexpr bool includeDebugInfo = true;
	const bool success = ShaderCompiler::compileShader(shaderFilePath, mainEntry, shaderTypeToTargetString(type), includeDebugInfo, compiledShader);

	if (!success)
	{
		return nullptr;
	}

	std::unique_ptr<RHI::RHIShader> newShader = RHI::PlatformRHI->createShader(compiledShader, type);
	if (!newShader)
	{
		return nullptr;
	}

	RHI::RHIShader* shaderPtr = newShader.get();
	mLoadedShaders[shaderFilePath] = std::move(newShader);

	return shaderPtr;
}

//TODO(KL): Find a better way to retrieve shaders than to lookup their original file path every time
RHI::RHIShader* KGXShaderCache::getLoadedShader(const std::string& originalShaderFilePath) const
{
	if (const auto foundShader = mLoadedShaders.find(originalShaderFilePath); foundShader != mLoadedShaders.end())
	{
		return foundShader->second.get();
	}

	return nullptr;
}
}
