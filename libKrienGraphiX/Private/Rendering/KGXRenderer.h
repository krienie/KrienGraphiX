
#pragma once

#include <memory>

#include "Private/RHI/RHIGraphicsCommandList.h"

namespace kgx::RHI
{
class RHIDepthStencilBuffer;
class RHITexture2D;
}

namespace kgx::rendering
{
class KGXRenderer
{
public:
    KGXRenderer(const KGXViewport& Viewport, RHI::RHITexture2D* OutputRenderTarget, RHI::RHIDepthStencilBuffer* DepthStencil);
    ~KGXRenderer() = default;

    void RenderFrame();

private:
    KGXViewport mViewport;

    //TODO(KL): Should probably implement some sort of resource handle/reference system instead of passing these pointers
    RHI::RHITexture2D* mOutputRenderTarget;

    //TODO(KL): Implement a texture pool where textures can be re-used
    RHI::RHITexture2D* mDepthStencil;
};
}
