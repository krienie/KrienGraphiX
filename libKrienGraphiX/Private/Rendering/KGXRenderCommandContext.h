
#pragma once

#include "KGXRenderCommandContextUtils.h"
#include "Private/Core/RenderThread.h"
#include "Private/Rendering/KGXRenderPass.h"
#include "Private/RHI/RHIResourceView.h"

namespace kgx::rendering
{
class KGXRenderScene;

class KGXRenderCommandContext
{
public:
	KGXRenderCommandContext(core::FrameCommandContext& frameContext);
	//~KGXRenderCommandContext();

	TextureHandle registerTexture(RHI::RHIResourceView* texture);

	void addRenderPass(const KGXRenderPassParameters& renderPassParameters);
	void runPasses();

private:
	[[nodiscard]] RHI::RHIResourceView* resolveTextureHandle(TextureHandle textureHandle) const;
	void executeRenderPass(const KGXRenderPassParameters& renderPassParameters);

	std::vector<KGXRenderPassParameters> mRenderPasses;
	std::vector<RHI::RHIResourceView*> mTextureRegistry;

	KGXRenderScene* mRenderScene = nullptr;
	core::FrameCommandContext& mFrameContext;
};
}
