
#include "KGXRenderCommandContext.h"

#include "KGXDrawPackage.h"
#include "KGXRenderPass.h"
#include "KGXRenderScene.h"
#include "Private/Core/RenderCore.h"

namespace kgx::rendering
{
KGXRenderCommandContext::KGXRenderCommandContext(core::FrameCommandContext& frameContext)
	: mFrameContext(frameContext)
{
	mRenderScene = core::RenderCore::get()->getScenePtr()->getRenderScenePtr();
}

TextureHandle KGXRenderCommandContext::registerTexture(RHI::RHIResourceView* texture)
{
	TextureHandle handle;
	handle.index = mTextureRegistry.size();
	mTextureRegistry.push_back(texture);

	return handle;
}

RHI::RHIResourceView* KGXRenderCommandContext::resolveTextureHandle(TextureHandle textureHandle) const
{
	if (textureHandle.isValid())
	{
		assert(textureHandle.index < mTextureRegistry.size());
		return mTextureRegistry[textureHandle.index];
	}

	return nullptr;
}

KGXRenderPass* KGXRenderCommandContext::addNewRenderPass(const std::string& passName)
{
	return &mRenderPasses.emplace_back(passName, *this);
}

void KGXRenderCommandContext::runCommands()
{
	if (!validateRenderPasses())
	{
		KGXLOG_ERROR("Error validating renderpasses. Aborting runCommands()");
		return;
	}

	if (mRenderPasses.empty())
	{
		KGXLOG_WARN("No RenderPasses are registered.");
		return;
	}

	auto sceneConstantBuffer = mRenderScene->updateAndGetSceneConstantBuffer();
	mFrameContext.getCommandList()->setConstantBuffer(sceneConstantBuffer, 0);

	for (auto& pass : mRenderPasses)
	{
		pass.runPass(mFrameContext);
	}
}

bool KGXRenderCommandContext::validateRenderPasses() const
{
	for (auto& renderPass : mRenderPasses)
	{
		if (!renderPass.isInitialized())
		{
			KGXLOG_ERROR("Error! RenderPass {} is not initialized.", renderPass.getPassName());
			return false;
		}
	}

	// - Make a graph of input and output textures of all registered renderpasses
	// - optional: purge renderpasses that do not contribute to the final output texture
	// - optional: allow culled renderpasses to not be culled
	// - run remaining renderpasses in order


	// Validation pass
	// Graph building:
	// - Walk from back to front, recursively. Only the renderpasses that are left over are kept.
	// - The last added renderpass is assumed to be the final output pass

	/*std::vector<KGXRenderPass*> liveRenderPasses;
	liveRenderPasses.reserve(mRenderPasses.size());
	liveRenderPasses.push_back(&mRenderPasses.back());

	// Walking back to front, skipping the renderpass at the back
	for (int i = static_cast<int>(mRenderPasses.size()) - 2; i >= 0; --i)
	{

	}*/

	return true;
}
}
