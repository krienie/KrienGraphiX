
#include "RcDepthStencilBuffer.h"

#include "Private/RHI/RenderHardwareInterface.h"
#include "Private/Core/RenderCore.h"

namespace kgx::Rendering
{
RcDepthStencilBuffer::RcDepthStencilBuffer(unsigned int width, unsigned int height)
    : mRHIDepthStencilBuffer(nullptr)
{
    auto * renderEngine = core::RenderCore::get();
    mRHIDepthStencilBuffer =
        renderEngine->getRHI()->createDepthStencilBuffer(renderEngine->getGraphicsDevicePtr(), width, height);
}

}
