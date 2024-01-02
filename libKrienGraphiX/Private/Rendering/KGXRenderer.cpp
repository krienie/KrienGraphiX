
#include "KGXRenderer.h"

#include <cassert>

#include "Private/Core/RenderCore.h"
#include "Private/RHI/RHIDescriptors.h"
#include "Private/RHI/RenderHardwareInterface.h"

namespace kgx::rendering
{
KGXRenderer::KGXRenderer(const core::KGXViewport& Viewport, RHI::RHIResourceView& OutputRenderTarget, RHI::RHIResourceView& DepthStencil)
    : mViewport(Viewport), mOutputRTV(OutputRenderTarget), mDSV(DepthStencil)
{
}

void KGXRenderer::RenderFrame()
{
    const auto* renderThread = core::RenderCore::get()->getRenderThreadPtr();

    //TODO(KL): Update RendererScene objects

    auto* commandList = renderThread->getGraphicsCommandListPtr();

    auto* OutputRenderTarget = static_cast<RHI::RHITexture2D*>(mOutputRTV.getViewedResource());

    RHI::PlatformRHI->beginFrame(commandList, OutputRenderTarget);
    
    commandList->setViewport(mViewport);
    
    //TODO(KL): temporary fixed clear color
    static float lightSteelBlue[4] = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };

    // Clear the back buffer and depth buffer.
    commandList->clearRenderTargetView(&mOutputRTV, lightSteelBlue);
    commandList->clearDepthStencilView(&mDSV, RHI::DepthStencilFlags::DepthStencilClear, 1.0f, 0);

    // Specify the buffers we are going to render to.
    commandList->setRenderTargets({ &mOutputRTV }, &mDSV);

    RHI::PlatformRHI->endFrame(commandList, OutputRenderTarget);

    renderThread->getCommandQueuePtr()->executeCommandList(commandList);
}
}
