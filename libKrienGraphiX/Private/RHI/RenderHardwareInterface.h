
#pragma once

#include "RHIBuffer.h"
#include "RHICommandQueue.h"
#include "RHIGraphicsCommandList.h"
#include "RHIShader.h"
#include "RHISwapChain.h"

#include <memory>

namespace kgx::RHI
{
class RenderHardwareInterface
{
public:
	RenderHardwareInterface() = default;
	virtual ~RenderHardwareInterface() = default;

	RenderHardwareInterface(const RenderHardwareInterface&) noexcept            = delete;
	RenderHardwareInterface& operator=(const RenderHardwareInterface&) noexcept = delete;
	RenderHardwareInterface(RenderHardwareInterface&&) noexcept                 = delete;
	RenderHardwareInterface& operator=(RenderHardwareInterface&&) noexcept      = delete;

	virtual void beginFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget) = 0;
	virtual void endFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget) = 0;

	[[nodiscard]]
	virtual std::unique_ptr<RHICommandQueue> createCommandQueue() = 0;

	[[nodiscard]]
	virtual std::unique_ptr<RHISwapChain> createSwapChain(
		RHICommandQueue* commandQueue,
		WinHandle windowHandle,
		unsigned int width,
		unsigned int height,
		unsigned int frameCount) = 0;

	[[nodiscard]]
	virtual std::unique_ptr<RHIShader> createShader(RHIGraphicsCommandList* commandList, const CompiledShader& compiledShader, RHIShader::ShaderType type) = 0;

	//TODO(KL): Remove the need to pass CommandListAllocator
	[[nodiscard]]
	virtual std::shared_ptr<RHIGraphicsCommandList> createGraphicsCommandList(core::CommandListAllocator* allocator, RHICommandQueue* commandQueue, RHIGraphicsPipelineState *pipelineState) = 0;

	//TODO(KL): Replace with createTexture()
	[[nodiscard]]
	virtual std::unique_ptr<RHITexture2D> createDepthStencilBuffer(RHITexture2DDescriptor descriptor) = 0;

	[[nodiscard]]
	virtual std::shared_ptr<RHIResourceView> createResourceView(RHIResourceView::Type type, const std::shared_ptr<RHIViewableResource>& viewedResource, bool isShaderVisible) = 0;

	[[nodiscard]]
	virtual std::unique_ptr<RHIGraphicsPipelineState> createGraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc) = 0;

	[[nodiscard]]
	virtual std::unique_ptr<RHIBuffer> createBuffer(RHIGraphicsCommandList* commandList, const RHIBufferDescriptor& descriptor) = 0;
};

inline std::unique_ptr<RenderHardwareInterface> PlatformRHI = nullptr;
}
