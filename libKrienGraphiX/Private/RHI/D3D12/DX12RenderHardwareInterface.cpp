
#include "DX12RenderHardwareInterface.h"

#include "DX12Buffer.h"
#include "DX12CommandAllocator.h"
#include "DX12CommandQueue.h"
#include "DX12Descriptors.h"
#include "DX12Fence.h"
#include "DX12GraphicsCommandList.h"
#include "DX12GraphicsDevice.h"
#include "DX12GraphicsPipelineState.h"
#include "DX12Shader.h"
#include "DX12SwapChain.h"
#include "DX12Texture2D.h"
#include "Private/Core/RenderCore.h"

namespace kgx::RHI
{
//TODO(KL): log errors when any of these inits fail

DX12RenderHardwareInterface::DX12RenderHardwareInterface()
{
	mGraphicsDevice = std::make_unique<DX12GraphicsDevice>();
	mGraphicsDevice->create();
}

DX12RenderHardwareInterface::~DX12RenderHardwareInterface()
{
	mGraphicsDevice.reset();
}

void DX12RenderHardwareInterface::beginFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget)
{
	DX12Texture2D* dxTexture2D = rcCast(renderTarget);
	dxTexture2D->transitionToState(rcCast(commandList), D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void DX12RenderHardwareInterface::endFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget)
{
	DX12Texture2D* dxTexture2D = rcCast(renderTarget);
	dxTexture2D->transitionToState(rcCast(commandList), D3D12_RESOURCE_STATE_PRESENT);
}

std::unique_ptr<RHICommandQueue> DX12RenderHardwareInterface::createCommandQueue() const
{
	auto commandQueue = std::make_unique<DX12CommandQueue>();
	if (!commandQueue->create())
	{
		return nullptr;
	}

	return commandQueue;
}

std::unique_ptr<RHISwapChain> DX12RenderHardwareInterface::createSwapChain(
	RHICommandQueue* commandQueue,
	SDL_Window* windowHandle,
	unsigned int width,
	unsigned int height,
	unsigned int frameCount) const
{
	auto swapChain = std::make_unique<DX12SwapChain>(width, height);
	if (!swapChain->create(commandQueue, windowHandle, frameCount, RHIPixelFormat::R10G10B10A2_unorm))
	{
		return nullptr;
	}

	return swapChain;
}

std::unique_ptr<RHIFence> DX12RenderHardwareInterface::createFence() const
{
	return std::make_unique<DX12Fence>();
}

std::unique_ptr<RHIShader> DX12RenderHardwareInterface::createShader(const CompiledShader& compiledShader, RHIShader::ShaderType type) const
{
	auto newShader = std::make_unique<DX12Shader>();
	if (!newShader->create(compiledShader, type))
	{
		return nullptr;
	}

	return newShader;
}

std::unique_ptr<RHICommandAllocator> DX12RenderHardwareInterface::createCommandAllocator() const
{
	return std::make_unique<DX12CommandAllocator>();
}

std::unique_ptr<RHIGraphicsCommandList> DX12RenderHardwareInterface::createGraphicsCommandList(RHIGraphicsPipelineState* pipelineState) const
{
	auto graphicsCommandList = std::make_unique<DX12GraphicsCommandList>();
	if (!graphicsCommandList->create(pipelineState))
	{
		return nullptr;
	}

	return graphicsCommandList;
}

std::unique_ptr<RHITexture2D> DX12RenderHardwareInterface::createDepthStencilBuffer(RHITexture2DDescriptor descriptor) const
{
	//TODO(KL): Validate pixel format

	DX12Texture2DDescriptor dx12Desc =
	{
		descriptor,
		nullptr,
		nullptr,
		0,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	};

	auto depthStencilBuffer = std::make_unique<DX12Texture2D>(dx12Desc);

	return depthStencilBuffer;
}

std::shared_ptr<RHIResourceView> DX12RenderHardwareInterface::createResourceView(RHIResourceView::Type type, const std::shared_ptr<RHIViewableResource>& viewedResource, bool isShaderVisible) const
{
	return std::make_shared<DX12ResourceView>(type, viewedResource, isShaderVisible);
}

std::unique_ptr<RHIGraphicsPipelineState> DX12RenderHardwareInterface::createGraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc) const
{
	auto graphicsPipelineState = std::make_unique<DX12GraphicsPipelineState>(desc);
	if (!graphicsPipelineState->create())
	{
		return nullptr;
	}

	return graphicsPipelineState;
}

std::unique_ptr<RHIBuffer> DX12RenderHardwareInterface::createBuffer(RHIGraphicsCommandList* commandList, const RHIBufferDescriptor& descriptor) const
{
	DX12GraphicsCommandList* dxCommandList = rcCast(commandList);

	return std::make_unique<DX12Buffer>(dxCommandList, descriptor);
}
}
