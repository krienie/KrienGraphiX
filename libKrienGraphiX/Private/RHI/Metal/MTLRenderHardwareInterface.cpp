
#include "MTLRenderHardwareInterface.h"

#include <cassert>
#include <SDL3/SDL.h>

#include "MTLBuffer.h"
#include "MTLCommandAllocator.h"
#include "MTLFence.h"
#include "MTLGraphicsPipelineState.h"
#include "MTLPlatform.h"
#include "MTLShader.h"
#include "MTLSwapChain.h"
#include "MTLTexture2D.h"
#include "KrienGraphiX/Core/Logging.h"

#include "Private/Core/RenderCore.h"

namespace kgx::RHI
{
//TODO(KL): log errors when any of these inits fail

MTLRenderHardwareInterface::MTLRenderHardwareInterface()
{
	mGraphicsDevice = std::make_unique<MTLGraphicsDevice>();
}

MTLRenderHardwareInterface::~MTLRenderHardwareInterface()
{
	mGraphicsDevice.reset();
}

std::unique_ptr<RHIPlatform> MTLRenderHardwareInterface::createPlatform() const
{
	return std::make_unique<MTLPlatform>();
}

std::unique_ptr<RHISwapChain> MTLRenderHardwareInterface::createSwapChain(
	SDL_Window* window,
	unsigned int width,
	unsigned int height,
	unsigned int frameCount) const
{
	auto swapChain = std::make_unique<MTLSwapChain>(width, height);
	if (!swapChain->create(window, frameCount, RHIPixelFormat::R10G10B10A2_unorm))
	{
		KGXLOG_ERROR("[MTL] Failed to create SwapChain");
		return nullptr;
	}

	return swapChain;
}

std::unique_ptr<RHIFence> MTLRenderHardwareInterface::createFence(RHIPlatform& platform) const
{
	return std::make_unique<MTLFence>(static_cast<MTLPlatform&>(platform));
}

std::unique_ptr<RHIShader> MTLRenderHardwareInterface::createShader(const CompiledShader& compiledShader, RHIShader::ShaderType type) const
{
	auto newShader = std::make_unique<MTLShader>();
	if (!newShader->create(compiledShader, type))
	{
		KGXLOG_ERROR("[MTL] Failed to create Shader");
		return nullptr;
	}

	return newShader;
}

std::unique_ptr<RHITexture2D> MTLRenderHardwareInterface::createDepthStencilBuffer(const RHITexture2DDescriptor& descriptor) const
{
	//TODO(KL): Implement
	assert(false);
	return nullptr;

	//TODO(KL): Validate pixel format

	/*DX12Texture2DDescriptor dx12Desc =
	{
		descriptor,
		nullptr,
		nullptr,
		0,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	};

	auto depthStencilBuffer = std::make_unique<DX12Texture2D>(dx12Desc);

	return depthStencilBuffer;*/
}

RHITextureHandle MTLRenderHardwareInterface::createTexture2D(const RHITexture2DDescriptor& descriptor) const
{
	auto newTexture = std::make_unique<MTLTexture2D>(MTLTexture2DDescriptor{descriptor});
	auto rhiTexture = std::unique_ptr<RHITexture2D>{static_cast<RHITexture2D*>(newTexture.release())};

	return core::gRenderThread->getRHIPlatformPtr()->registerTexture(std::move(rhiTexture));
}

std::shared_ptr<RHIResourceView> MTLRenderHardwareInterface::createResourceView(RHIResourceView::Type type, const std::shared_ptr<RHIViewableResource>& viewedResource, bool isShaderVisible) const
{
	//TODO(KL): DSV only for now
	assert(type == RHIResourceView::Type::DSV);
	return std::make_shared<MTLTextureView>(type, viewedResource);
}

std::unique_ptr<RHIGraphicsPipelineState> MTLRenderHardwareInterface::createGraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc) const
{
	auto graphicsPipelineState = std::make_unique<MTLGraphicsPipelineState>(desc);
	if (!graphicsPipelineState->create())
	{
		KGXLOG_ERROR("[MTL] Failed to create GraphicsPipelineState");
		return nullptr;
	}

	return graphicsPipelineState;
}

std::unique_ptr<RHIBuffer> MTLRenderHardwareInterface::createBuffer(RHIRenderContext* renderContext, const RHIBufferDescriptor& descriptor) const
{
	return std::make_unique<MTLBuffer>(descriptor);
}
}
