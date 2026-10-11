
#pragma once

#include "Private/Core/PrivateDefinitions.h"
#include "Private/RHI/RHITextureHandle.h"

namespace kgx::rendering
{
class KGXRenderer final
{
public:
	KGXRenderer(
		const core::KGXViewport& Viewport,
		const RHI::RHITextureHandle& OutputRenderTarget,
		const RHI::RHITextureHandle& DepthStencil);
	~KGXRenderer() = default;

	void RenderFrame() const;

private:
	core::KGXViewport mViewport;
	RHI::RHITextureHandle mOutputRTV;
	RHI::RHITextureHandle mDSV;
};
}
