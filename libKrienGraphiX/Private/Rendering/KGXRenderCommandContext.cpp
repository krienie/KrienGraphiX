
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
	auto meshInstanceConstantBuffer = mRenderScene->getMeshInstanceConstantBuffer();
	auto objectIdsBuffer = mRenderScene->getObjectIdsBuffer();
	mFrameContext.getRenderContext()->setConstantBuffers(sceneConstantBuffer, meshInstanceConstantBuffer, objectIdsBuffer);

	for (auto& pass : mRenderPasses)
	{
		executeRenderPass(pass);
	}
}

void KGXRenderCommandContext::executeRenderPass(const KGXRenderPassParameters& renderPassParameters)
{
	mFrameContext.getRenderContext()->activateRenderPass(renderPassParameters);

	for (auto& renderObject : *mRenderScene)
	{
		// Update the renderObject's instance data
		auto* instances = static_cast<MeshInstanceData*>(mRenderScene->getMeshInstanceConstantBuffer()->mappedDataPtr());
		instances[renderObject->getObjectId()] = MeshInstanceData{ renderObject->getTransform() };

		mFrameContext.getRenderContext()->drawMeshRenderObject(renderObject.get());
	}
}
}
