
#pragma once

#include "Private/Core/RenderThread.h"
#include "Private/Rendering/KGXRenderPass.h"

namespace kgx::rendering
{
class KGXRenderScene;

class KGXRenderCommandContext final
{
public:
	KGXRenderCommandContext(core::FrameCommandContext& frameContext);

	void addRenderPass(const KGXRenderPassParameters& renderPassParameters);
	void runPasses() const;

private:
	void executeRenderPass(const KGXRenderPassParameters& renderPassParameters) const;

	std::vector<KGXRenderPassParameters> mRenderPasses;

	KGXRenderScene* mRenderScene = nullptr;
	core::FrameCommandContext& mFrameContext;
};
}
