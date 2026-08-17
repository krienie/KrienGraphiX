
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
	handle.index = static_cast<int>(mTextureRegistry.size());
	mTextureRegistry.push_back(texture);

	return handle;
}

void KGXRenderCommandContext::addRenderPass(const KGXRenderPassParameters& renderPassParameters)
{
	mRenderPasses.push_back(renderPassParameters);
}

void KGXRenderCommandContext::runPasses()
{
	if (mRenderPasses.empty())
	{
		KGXLOG_WARN("No RenderPasses are registered.");
		return;
	}

	auto sceneConstantBuffer = mRenderScene->updateAndGetSceneConstantBuffer();
	mFrameContext.getCommandList()->setConstantBuffer(sceneConstantBuffer, 0);

	for (auto& pass : mRenderPasses)
	{
		executeRenderPass(pass);
	}
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

void KGXRenderCommandContext::executeRenderPass(const KGXRenderPassParameters& renderPassParameters)
{
	auto* commandList = mFrameContext.getCommandList();

	commandList->setViewport(renderPassParameters.viewport);

	//TODO(KL): directly pass in the TextureBindings to the commandlist and let the RHI handle it

	std::vector<RHI::RHIResourceView*> renderTargets;
	renderTargets.reserve(renderPassParameters.outputTextures.size());
	for (auto& renderTargetBinding : renderPassParameters.outputTextures)
	{
		if (auto* texture = resolveTextureHandle(renderTargetBinding.texture))
		{
			if (renderTargetBinding.loadAction == TextureLoadAction::Clear)
			{
				commandList->clearRenderTargetView(texture, renderTargetBinding.clearColor.data());
			}

			renderTargets.emplace_back(texture);
		}
	}
	//TODO(KL): implement clearDepthStencilView
	//commandList->clearDepthStencilView(mDSV, RHI::DepthStencilFlags::DepthStencilClear, 1.0f, 0);

	commandList->setRenderTargets(renderTargets, renderPassParameters.depthStencilView);

	commandList->setPipelineState(renderPassParameters.pso);

	for (auto& renderObject : *mRenderScene)
	{
		renderObject->updateConstantBufferData();
		commandList->setConstantBuffer(renderObject->getConstantBuffer(), 1);

		commandList->drawMeshRenderObject(renderObject.get());
	}
}
}
