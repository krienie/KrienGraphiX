
#include "MTLRenderHardwareInterface.h"

#include "MTLGraphicsDevice.h"
//#include "MTLCommandQueue.h"
//#include "MTLGraphicsCommandList.h"
//#include "MTLGraphicsPipelineState.h"
//#include "MTLShader.h"
//#include "MTLSwapChain.h"

#include <cassert>

//#include "MTLBuffer.h"
//#include "MTLDescriptors.h"
//#include "MTLTexture2D.h"
#include "Private/Core/RenderCore.h"

namespace kgx::RHI
{
//TODO(KL): log errors when any of these inits fail

MTLRenderHardwareInterface::MTLRenderHardwareInterface()
{
	mGraphicsDevice = std::make_unique<MTLGraphicsDevice>();
	mGraphicsDevice->create();
}

void MTLRenderHardwareInterface::beginFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget)
{
	//TODO(KL): Implement
	assert(false);
	//DX12Texture2D* dxTexture2D = dxCast(renderTarget);
	//dxTexture2D->transitionToState(dxCast(commandList), D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void MTLRenderHardwareInterface::endFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget)
{
	//TODO(KL): Implement
	assert(false);
	//DX12Texture2D* dxTexture2D = dxCast(renderTarget);
	//dxTexture2D->transitionToState(dxCast(commandList), D3D12_RESOURCE_STATE_PRESENT);
}

std::unique_ptr<RHICommandQueue> MTLRenderHardwareInterface::createCommandQueue() const
{
	//TODO(KL): Implement
	assert(false);
	return nullptr;

	/*auto commandQueue = std::make_unique<DX12CommandQueue>();
	if (!commandQueue->create())
	{
		return nullptr;
	}

	return std::move(commandQueue);*/
}

std::unique_ptr<RHISwapChain> MTLRenderHardwareInterface::createSwapChain(
	RHICommandQueue* commandQueue,
	WinHandle windowHandle,
	unsigned int width,
	unsigned int height,
	unsigned int frameCount) const
{
	//TODO(KL): Implement
	assert(false);
	return nullptr;

	/*auto swapChain = std::make_unique<DX12SwapChain>(width, height);
	if (!swapChain->create(commandQueue, windowHandle, frameCount, RHIPixelFormat::R10G10B10A2_unorm))
	{
		return nullptr;
	}

	return std::move(swapChain);*/
}

std::unique_ptr<RHIShader> MTLRenderHardwareInterface::createShader(const CompiledShader& compiledShader, RHIShader::ShaderType type) const
{
	//TODO(KL): Implement
	assert(false);
	return nullptr;

	/*auto newShader = std::make_unique<DX12Shader>();
	if (!newShader->create(compiledShader, type))
	{
		return nullptr;
	}

	return std::move(newShader);*/
}

std::shared_ptr<RHIGraphicsCommandList> MTLRenderHardwareInterface::createGraphicsCommandList(core::CommandListAllocator* allocator, RHIGraphicsPipelineState* pipelineState) const
{
	//TODO(KL): Implement
	assert(false);
	return nullptr;

	/*auto graphicsCommandList = std::make_shared<DX12GraphicsCommandList>(allocator);
	if (!graphicsCommandList->create(pipelineState))
	{
		return nullptr;
	}

	return graphicsCommandList;*/
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
	//TODO(KL): Implement
	assert(false);
	return nullptr;

	//return std::make_shared<DX12ResourceView>(type, viewedResource, isShaderVisible);
}

std::unique_ptr<RHIGraphicsPipelineState> MTLRenderHardwareInterface::createGraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc) const
{
	//TODO(KL): Implement
	assert(false);
	return nullptr;

	/*auto graphicsPipelineState = std::make_unique<DX12GraphicsPipelineState>(desc);
	if (!graphicsPipelineState->create())
	{
		return nullptr;
	}

	return std::move(graphicsPipelineState);*/
}

std::unique_ptr<RHIBuffer> MTLRenderHardwareInterface::createBuffer(RHIGraphicsCommandList* commandList, const RHIBufferDescriptor& descriptor) const
{
	//TODO(KL): Implement
	assert(false);
	return nullptr;

	/*DX12GraphicsCommandList* dxCommandList = dxCast(commandList);

	auto newBuffer = std::make_unique<DX12Buffer>(dxCommandList, descriptor);
	return std::move(newBuffer);*/
}
}
