
#pragma once

#include "Private/RHI/RenderHardwareInterface.h"

namespace kgx::RHI
{
class DX12RenderHardwareInterface : public RenderHardwareInterface
{
public:
	DX12RenderHardwareInterface();
	~DX12RenderHardwareInterface() override = default;

	DX12RenderHardwareInterface(const DX12RenderHardwareInterface&) noexcept            = delete;
	DX12RenderHardwareInterface& operator=(const DX12RenderHardwareInterface&) noexcept = delete;
	DX12RenderHardwareInterface(DX12RenderHardwareInterface&&) noexcept                 = delete;
	DX12RenderHardwareInterface& operator=(DX12RenderHardwareInterface&&) noexcept      = delete;

	void beginFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget) override;
	void endFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget) override;

	[[nodiscard]]
	std::unique_ptr<RHICommandQueue> createCommandQueue() const override;

	[[nodiscard]]
	std::unique_ptr<RHISwapChain> createSwapChain(
		RHICommandQueue * commandQueue,
		WinHandle windowHandle,
		unsigned int width,
		unsigned int height,
		unsigned int frameCount) const override;

	[[nodiscard]]
	std::unique_ptr<RHIShader> createShader(const CompiledShader& compiledShader, RHIShader::ShaderType type) const override;

	//TODO(KL): Remove the need for passing CommandListAllocator? integrate it into DX12RenderHardwareInterface
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

	class DX12GraphicsDevice* getDX12Device() const { return mGraphicsDevice.get(); }

private:
	std::unique_ptr<class DX12GraphicsDevice> mGraphicsDevice;
};

inline DX12RenderHardwareInterface* getDX12RHI()
{
	return static_cast<DX12RenderHardwareInterface*>(PlatformRHI.get());
}
}
