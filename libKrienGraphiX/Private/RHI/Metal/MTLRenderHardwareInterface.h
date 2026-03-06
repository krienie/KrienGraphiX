
#pragma once

#include "Private/RHI/RenderHardwareInterface.h"
#include "Private/RHI/Metal/MTLGraphicsDevice.h"

namespace kgx::RHI
{
class MTLRenderHardwareInterface : public RenderHardwareInterface
{
public:
	MTLRenderHardwareInterface();
	~MTLRenderHardwareInterface() override = default;

	MTLRenderHardwareInterface(const MTLRenderHardwareInterface&) noexcept            = delete;
	MTLRenderHardwareInterface& operator=(const MTLRenderHardwareInterface&) noexcept = delete;
	MTLRenderHardwareInterface(MTLRenderHardwareInterface&&) noexcept                 = delete;
	MTLRenderHardwareInterface& operator=(MTLRenderHardwareInterface&&) noexcept      = delete;

	void beginFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget) override;
	void endFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget) override;

	[[nodiscard]]
	std::unique_ptr<RHICommandQueue> createCommandQueue() const override;

	[[nodiscard]]
	std::unique_ptr<RHISwapChain> createSwapChain(
		RHICommandQueue* commandQueue,
		WinHandle windowHandle,
		unsigned int width,
		unsigned int height,
		unsigned int frameCount) const override;

	[[nodiscard]]
	std::unique_ptr<RHIShader> createShader(const CompiledShader& compiledShader, RHIShader::ShaderType type) const override;

	//TODO(KL): Remove the need for passing CommandListAllocator? integrate it into MTLRenderHardwareInterface
	[[nodiscard]]
	std::shared_ptr<RHIGraphicsCommandList> createGraphicsCommandList(core::CommandListAllocator* allocator, RHIGraphicsPipelineState *pipelineState) const override;

	[[nodiscard]]
	std::unique_ptr<RHITexture2D> createDepthStencilBuffer(RHITexture2DDescriptor descriptor) const override;

	[[nodiscard]]
	std::shared_ptr<RHIResourceView> createResourceView(RHIResourceView::Type type, const std::shared_ptr<RHIViewableResource>& viewedResource, bool isShaderVisible) const override;

	[[nodiscard]]
	std::unique_ptr<RHIGraphicsPipelineState> createGraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc) const override;

	[[nodiscard]]
	std::unique_ptr<RHIBuffer> createBuffer(RHIGraphicsCommandList* commandList, const RHIBufferDescriptor& descriptor) const override;

	[[nodiscard]]
	MTLGraphicsDevice* getMTLDevice() const { return mGraphicsDevice.get(); }

private:
	std::unique_ptr<MTLGraphicsDevice> mGraphicsDevice;
};

inline MTLRenderHardwareInterface* getMTLRHI()
{
	return static_cast<MTLRenderHardwareInterface*>(PlatformRHI.get());
}
}
