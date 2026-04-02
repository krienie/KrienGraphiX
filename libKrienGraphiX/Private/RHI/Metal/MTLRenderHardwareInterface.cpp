
#include "MTLRenderHardwareInterface.h"

#include <cassert>
#include <SDL3/SDL.h>

#include "MTLBuffer.h"
#include "MTLCommandAllocator.h"
#include "MTLCommandQueue.h"
#include "MTLFence.h"
#include "MTLGraphicsCommandList.h"
#include "MTLGraphicsPipelineState.h"
#include "MTLShader.h"
#include "MTLSwapChain.h"
#include "MTLTexture2D.h"

#include "Private/Core/RenderCore.h"

namespace kgx::RHI
{
//TODO(KL): log errors when any of these inits fail

MTLRenderHardwareInterface::MTLRenderHardwareInterface()
	: mAutoReleasePool(nullptr)
{
	mGraphicsDevice = std::make_unique<MTLGraphicsDevice>();
}

MTLRenderHardwareInterface::~MTLRenderHardwareInterface()
{
	if (mAutoReleasePool)
	{
		mAutoReleasePool->release();
	}

	mGraphicsDevice.reset();
}

void MTLRenderHardwareInterface::beginFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget)
{
	if (mAutoReleasePool)
	{
		mAutoReleasePool->release();
		mAutoReleasePool = nullptr;
	}
	mAutoReleasePool = NS::AutoreleasePool::alloc()->init();
}

void MTLRenderHardwareInterface::endFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget)
{
}

std::unique_ptr<RHICommandQueue> MTLRenderHardwareInterface::createCommandQueue() const
{
	auto commandQueue = std::make_unique<MTLCommandQueue>();
	if (!commandQueue->create())
	{
		return nullptr;
	}

	return std::move(commandQueue);
}

std::unique_ptr<RHISwapChain> MTLRenderHardwareInterface::createSwapChain(
	RHICommandQueue* commandQueue,
	SDL_Window* window,
	unsigned int width,
	unsigned int height,
	unsigned int frameCount) const
{
	auto swapChain = std::make_unique<MTLSwapChain>(width, height);
	if (!swapChain->create(commandQueue, window, frameCount, RHIPixelFormat::R10G10B10A2_unorm))
	{
		return nullptr;
	}

	return std::move(swapChain);
}

std::unique_ptr<RHIFence> MTLRenderHardwareInterface::createFence() const
{
	return std::make_unique<MTLFence>();
}

std::unique_ptr<RHIShader> MTLRenderHardwareInterface::createShader(const CompiledShader& compiledShader, RHIShader::ShaderType type) const
{
	auto newShader = std::make_unique<MTLShader>();
	if (!newShader->create(compiledShader, type))
	{
		return nullptr;
	}

	return std::move(newShader);
}

std::unique_ptr<RHICommandAllocator> MTLRenderHardwareInterface::createCommandAllocator() const
{
	return std::make_unique<MTLCommandAllocator>();
}

std::unique_ptr<RHIGraphicsCommandList> MTLRenderHardwareInterface::createGraphicsCommandList(RHIGraphicsPipelineState *pipelineState) const
{
	auto graphicsCommandList = std::make_unique<MTLGraphicsCommandList>();
	if (!graphicsCommandList->create(pipelineState))
	{
		return nullptr;
	}

	return graphicsCommandList;
}

std::unique_ptr<RHITexture2D> MTLRenderHardwareInterface::createDepthStencilBuffer(RHITexture2DDescriptor descriptor) const
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

	return std::move(depthStencilBuffer);*/
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
		return nullptr;
	}

	return std::move(graphicsPipelineState);
}

std::unique_ptr<RHIBuffer> MTLRenderHardwareInterface::createBuffer(RHIGraphicsCommandList* commandList, const RHIBufferDescriptor& descriptor) const
{
	auto newBuffer = std::make_unique<MTLBuffer>(descriptor);
	return std::move(newBuffer);
}
}
