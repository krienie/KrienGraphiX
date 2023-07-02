
#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Private/RHI/RHIShader.h"

namespace kgx::rendering
{
class KGXShaderCache final
{
    public:
        explicit KGXShaderCache(RHI::RHIGraphicsDevice* graphicsDevice);
        ~KGXShaderCache() = default;

        bool loadShaderFromFile(const std::string& shaderFilePath, const std::string& mainEntry, RHI::RHIShader::ShaderType type);

        RHI::RHIShader* getLoadedShader(const std::string& originalShaderFilePath) const;

    private:
        std::unordered_map<std::string, std::shared_ptr<RHI::RHIShader>> mLoadedShaders;

        RHI::RHIGraphicsDevice* mGraphicsDevice;
};
}
