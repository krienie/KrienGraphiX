
#include "KGXRenderPass.h"

#include "Private/Core/RenderCore.h"

namespace kgx::Rendering
{
KGXRenderPass::KGXRenderPass()
    : mCommandList(nullptr)
{
    const auto* renderEngine = core::RenderCore::get();
    //mCommandList = renderEngine->getRHI()->createGraphicsCommandList(renderEngine->getGraphicsDevicePtr(), nullptr);
}
}
