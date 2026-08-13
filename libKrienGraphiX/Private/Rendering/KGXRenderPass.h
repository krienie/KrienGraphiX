
#pragma once

#include <array>
#include <unordered_map>
#include <vector>

#include "KGXDrawPackage.h"
#include "KGXRenderCommandContextUtils.h"

namespace kgx::core
{
class FrameCommandContext;
}

namespace kgx::rendering
{
class KGXRenderCommandContext;
class KGXMeshRenderObject;

enum class TextureLoadAction
{
	DoNothing,
	Load,
	Clear
};

enum class TextureStoreAction
{
	DoNothing,
	Store
};

struct TextureBinding
{
	TextureHandle texture;
	TextureLoadAction loadAction = TextureLoadAction::DoNothing;
	TextureStoreAction storeAction = TextureStoreAction::DoNothing;
	std::array<float, 4> clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
};

class KGXRenderPass
{
public:
	KGXRenderPass(const std::string& passName, KGXRenderCommandContext& renderCommandContext);
	~KGXRenderPass() = default;

	//TODO(KL): Define a nice interface for defining I/O for the renderpass
	void initPass(const std::vector<TextureBinding>& inputTextures, const std::vector<TextureBinding>& outputTextures,
		const core::KGXViewport& viewport, RHI::RHIResourceView* depthStencilView);
	void drawMesh(RHI::RHIGraphicsPipelineState* pso, const KGXConstantBufferUpdatePackage& bufferUpdates, const KGXMeshRenderObject* renderObject);
	void runPass(core::FrameCommandContext& frameContext);

	[[nodiscard]] bool isInitialized() const;
	[[nodiscard]] std::string getPassName() const;
	[[nodiscard]] const std::vector<TextureBinding>& getInputTexturesRef() const;
	[[nodiscard]] const std::vector<TextureBinding>& getOutputTexturesRef() const;

private:
	struct ObjectDrawCall
	{
		KGXConstantBufferUpdatePackage bufferUpdates;
		const KGXMeshRenderObject* renderObject = nullptr;
	};

	std::unordered_map<XXH64_hash_t, int> mPsoHashToCacheIndex;
	std::vector<RHI::RHIGraphicsPipelineState*> mPsoCache;
	std::vector<std::vector<ObjectDrawCall>> mDrawCallCache;

	core::KGXViewport mViewport{};
	RHI::RHIResourceView* mDepthStencilView;
	std::vector<TextureBinding> mInputTextures;
	std::vector<TextureBinding> mOutputTextures;

	std::string mPassName;
	KGXRenderCommandContext& mRenderCommandContext;

	bool mIsInitialized = false;
};
}



