
#include "KGXRenderPass.h"

#include "Private/Core/RenderCore.h"

namespace kgx::rendering
{
KGXRenderPass::KGXRenderPass()
    : mCommandList(nullptr)
{
    const auto* renderEngine = core::RenderCore::get();
    //mCommandList = renderEngine->getRHI()->createGraphicsCommandList(renderEngine->getGraphicsDevicePtr(), nullptr);
}
}
