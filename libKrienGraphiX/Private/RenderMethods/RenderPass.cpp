
#include "RenderPass.h"

#include "Private/Core/RenderCore.h"

namespace kgx::RenderMethods
{
RenderPass::RenderPass()
    : mCommandList(nullptr)
{
    auto * renderEngine = core::RenderCore::get();
    mCommandList = renderEngine->getRHI()->createGraphicsCommandList(renderEngine->getGraphicsDevicePtr(), nullptr);
}
}
