
#pragma once

#include <array>
#include <memory>

#include "Private/RHI/RHIGraphicsCommandList.h"
#include "Private/RHI/RHIShader.h"

namespace kgx::RHI
{
class RHITexture2D;
}

namespace kgx::rendering
{
class KGXRenderPass final
{
public:
    KGXRenderPass();
    ~KGXRenderPass() = default;

    KGXRenderPass& setBlendState();
    KGXRenderPass& setDepthStencilState();
    KGXRenderPass& setRasterizerState();
    KGXRenderPass& setRenderTarget(const RHI::RHITexture2D& renderTarget, uint8_t slot);

    KGXRenderPass& setShader(const RHI::RHIShader& shader, RHI::RHIShader::ShaderType type);

    void execute();
    
private:
    std::unique_ptr<RHI::RHIGraphicsCommandList> mCommandList = nullptr;
    std::unordered_map<RHI::RHIShader::ShaderType, const RHI::RHIShader*> mBoundShaders;
    std::array<const RHI::RHITexture2D*, 6> mRenderTargets{};
};
}
