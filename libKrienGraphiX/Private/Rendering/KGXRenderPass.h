
#pragma once

#include <cstdint>
#include <vector>

#include "Private/Core/PrivateDefinitions.h"
#include "Private/RHI/RHIDefinitions.h"
#include "Private/RHI/RHIGraphicsPipelineState.h"
#include "Private/RHI/RHIResourceView.h"
#include "Private/RHI/RHITextureHandle.h"

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
	RHI::RHITextureHandle texture;
	TextureLoadAction loadAction = TextureLoadAction::DoNothing;
	TextureStoreAction storeAction = TextureStoreAction::DoNothing;
	RHI::RHIClearValue clearValue = { 0.0f, 0.0f, 0.0f, 0.0f };
};

enum class DepthComparisonFunction : uint8_t
{
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always
};

struct DepthStencilBinding final
{
	TextureBinding textureBinding;
	bool depthEnable = false;
	bool depthWriteEnable = false;
	DepthComparisonFunction depthFunction = DepthComparisonFunction::Always;
	bool stencilEnable = false;
	uint8_t stencilReadMask = 0xFF;
	uint8_t stencilWriteMask = 0xFF;
};

struct KGXRenderPassParameters final
{
	std::string name;
	core::KGXViewport viewport{};
	DepthStencilBinding depthStencilBinding;
	std::vector<TextureBinding> inputTextures;
	std::vector<TextureBinding> outputTextures;
	RHI::RHIGraphicsPipelineState* pso;
};
}



