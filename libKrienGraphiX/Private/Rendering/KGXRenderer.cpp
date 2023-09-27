
#include "KGXRenderer.h"

#include <cassert>

#include "Private/Core/RenderCore.h"
#include "Private/RHI/RenderHardwareInterface.h"

namespace kgx::rendering
{
KGXRenderer::KGXRenderer(const KGXViewport& Viewport, RHI::RHITexture2D* OutputRenderTarget, RHI::RHIDepthStencilBuffer* DepthStencil)
    : mViewport(Viewport), mOutputRenderTarget(OutputRenderTarget), mDepthStencil(DepthStencil)
{
    assert(mOutputRenderTarget != nullptr);
    assert(mDepthStencil != nullptr);
}

void KGXRenderer::RenderFrame()
{
    const auto* renderThread = core::RenderCore::get()->getRenderThreadPtr();

    auto* commandList = renderThread->getGraphicsCommandListPtr();

    RHI::PlatformRHI->beginFrame(commandList, mOutputRenderTarget);
    
    commandList->setViewport(mViewport);
    
    //TODO(KL): temporary fixed clear color
    static float lightSteelBlue[4] = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };

    auto backBufferRtv = mOutputRenderTarget->getResourceViewByType(RHI::RHIResourceView::RTV).get();
    auto depthStencilView = mDepthStencil->getResourceViewByType(RHI::RHIResourceView::DSV).get();

    // Clear the back buffer and depth buffer.
    commandList->clearRenderTargetView(backBufferRtv, lightSteelBlue);
    commandList->clearDepthStencilView(depthStencilView, RHI::RHIResourceView::DepthStencilFlags::DepthStencilClear, 1.0f, 0);

    // Specify the buffers we are going to render to.
    commandList->setRenderTargets({ backBufferRtv }, depthStencilView);

    RHI::PlatformRHI->endFrame(commandList, mOutputRenderTarget);

    renderThread->getCommandQueuePtr()->executeCommandList(commandList);
}
}
