
#pragma once

#include "Private/Core/RenderThread.h"
#include "Private/Rendering/KGXRenderPass.h"

namespace kgx::rendering
{
class KGXRenderScene;

class KGXRenderCommandContext
{
public:
	KGXRenderCommandContext(core::FrameCommandContext& frameContext);

	void addRenderPass(const KGXRenderPassParameters& renderPassParameters);
	void runPasses();

private:
	void executeRenderPass(const KGXRenderPassParameters& renderPassParameters);

	std::vector<KGXRenderPassParameters> mRenderPasses;

	KGXRenderScene* mRenderScene = nullptr;
	core::FrameCommandContext& mFrameContext;
};
}
