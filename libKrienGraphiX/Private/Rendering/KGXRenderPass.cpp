
#include "KGXRenderPass.h"

#include <cassert>

#include "Private/Core/RenderCore.h"

namespace kgx::rendering
{
KGXRenderPass::KGXRenderPass()
{
    const auto* renderEngine = core::RenderCore::get();
    //mCommandList = renderEngine->getRHI()->createGraphicsCommandList(renderEngine->getGraphicsDevicePtr(), nullptr);

    std::memset(mRenderTargets.data(), 0, sizeof(mRenderTargets));
}

KGXRenderPass& KGXRenderPass::setRenderTarget(const RHI::RHITexture2D& renderTarget, uint8_t slot)
{
    assert(slot < 6);

    mRenderTargets[slot] = &renderTarget;
    return *this;
}

KGXRenderPass& KGXRenderPass::setShader(const RHI::RHIShader& shader, RHI::RHIShader::ShaderType type)
{
    mBoundShaders[type] = &shader;
    return *this;
}
}
