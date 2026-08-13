
#include "KGXRenderPass.h"

#include <xxh3.h>

#include "KGXMeshRenderObject.h"
#include "KGXRenderCommandContext.h"
#include "Private/Core/RenderThread.h"

namespace kgx::rendering
{
KGXRenderPass::KGXRenderPass(const std::string& passName, KGXRenderCommandContext& renderCommandContext)
	: mDepthStencilView(nullptr), mPassName(passName), mRenderCommandContext(renderCommandContext)
{
}

void KGXRenderPass::drawMesh(RHI::RHIGraphicsPipelineState* pso, const KGXConstantBufferUpdatePackage& bufferUpdates, const KGXMeshRenderObject* renderObject)
{
	assert(pso);
	XXH64_hash_t psoHash = kgx::hash(*pso);

	int cacheIndex = -1;

	auto cacheIt = mPsoHashToCacheIndex.find(psoHash);
	if (cacheIt != mPsoHashToCacheIndex.end())
	{
		cacheIndex = cacheIt->second;
	}
	else
	{
		mPsoCache.push_back(pso);
		cacheIndex = static_cast<int>(mDrawCallCache.size());
		mDrawCallCache.emplace_back();
		mPsoHashToCacheIndex[psoHash] = cacheIndex;
	}

	mDrawCallCache[cacheIndex].push_back({bufferUpdates, renderObject});
}

void KGXRenderPass::runPass(core::FrameCommandContext& frameContext)
{
	auto* commandList = frameContext.getCommandList();

	commandList->setViewport(mViewport);

	//TODO(KL): directly pass in the TextureBindings to the commandlist and let the RHI handle it

	std::vector<RHI::RHIResourceView*> renderTargets;
	renderTargets.reserve(mOutputTextures.size());
	for (auto& renderTargetBinding : mOutputTextures)
	{
		if (auto* texture = mRenderCommandContext.resolveTextureHandle(renderTargetBinding.texture))
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

	commandList->setRenderTargets(renderTargets, mDepthStencilView);

	for (int i = 0; i < mPsoCache.size(); ++i)
	{
		auto* pso = mPsoCache[i];
		commandList->setPipelineState(pso);

		auto& psoDrawCalls = mDrawCallCache[i];

		for (auto& drawCall : psoDrawCalls)
		{
			//TODO(KL): Do something with bufferUpdates?
			//drawCall.bufferUpdates

			drawCall.renderObject->updateConstantBufferData();
			commandList->setConstantBuffer(drawCall.renderObject->getConstantBuffer(), 1);

			commandList->drawMeshRenderObject(drawCall.renderObject);
		}
	}
}

bool KGXRenderPass::isInitialized() const
{
	return mIsInitialized;
}

std::string KGXRenderPass::getPassName() const
{
	return mPassName;
}

const std::vector<TextureBinding>& KGXRenderPass::getInputTexturesRef() const
{
	return mInputTextures;
}

const std::vector<TextureBinding>& KGXRenderPass::getOutputTexturesRef() const
{
	return mOutputTextures;
}

//TODO(KL): Use TextureBinding for dsv as well
void KGXRenderPass::initPass(const std::vector<TextureBinding>& inputTextures, const std::vector<TextureBinding>& outputTextures,
							const core::KGXViewport& viewport, RHI::RHIResourceView* depthStencilView)
{
	//TODO(KL): Make DSV part of the outputTextures
	assert(!outputTextures.empty() || depthStencilView);

	mViewport = viewport;
	mDepthStencilView = depthStencilView;
	mInputTextures = inputTextures;
	mOutputTextures = outputTextures;

	mIsInitialized = true;
}
}
