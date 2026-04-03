
#pragma once

#include "Private/Core/PrivateDefinitions.h"
#include "Private/RHI/RHIResourceView.h"

namespace kgx::RHI
{
class RHITexture2D;
}

namespace kgx::rendering
{
class KGXRenderer
{
public:
	KGXRenderer(const core::KGXViewport& Viewport, RHI::RHIResourceView& OutputRenderTarget, RHI::RHIResourceView& DepthStencil);
	~KGXRenderer() = default;

	void RenderFrame();

private:
	core::KGXViewport mViewport;
	RHI::RHIResourceView* mOutputRTV;

	//TODO(KL): Implement a texture pool where textures can be re-used
	RHI::RHIResourceView* mDSV;
};
}
