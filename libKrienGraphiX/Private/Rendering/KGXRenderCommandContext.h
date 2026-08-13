
#pragma once

#include <unordered_map>

#include "KGXRenderCommandContextUtils.h"
#include "Private/Core/RenderThread.h"
#include "Private/RHI/RHITexture2D.h"

namespace kgx::rendering
{
class KGXRenderPass;
class KGXRenderScene;

class KGXRenderCommandContext
{
public:
	KGXRenderCommandContext(core::FrameCommandContext& frameContext);
	//~KGXRenderCommandContext();

	TextureHandle registerTexture(RHI::RHIResourceView* texture);
	[[nodiscard]] RHI::RHIResourceView* resolveTextureHandle(TextureHandle textureHandle) const;

	KGXRenderPass* addNewRenderPass(const std::string& passName);

	void runCommands();

private:
	[[nodiscard]] bool validateRenderPasses() const;

	//TODO(KL): Make sure the renderpasses can be sorted by I/O textures
	std::vector<KGXRenderPass> mRenderPasses;
	std::vector<RHI::RHIResourceView*> mTextureRegistry;

	KGXRenderScene* mRenderScene = nullptr;
	core::FrameCommandContext& mFrameContext;
};
}
