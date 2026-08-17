
#pragma once

#include <array>
#include <vector>

#include "KGXRenderCommandContextUtils.h"
#include "Private/Core/PrivateDefinitions.h"
#include "Private/RHI/RHIGraphicsPipelineState.h"
#include "Private/RHI/RHIResourceView.h"

namespace kgx::rendering
{
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

struct KGXRenderPassParameters
{
	std::string name;
	core::KGXViewport viewport{};
	RHI::RHIResourceView* depthStencilView;
	std::vector<TextureBinding> inputTextures;
	std::vector<TextureBinding> outputTextures;
	RHI::RHIGraphicsPipelineState* pso;
};
}



