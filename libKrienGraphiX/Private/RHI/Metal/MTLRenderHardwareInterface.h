
#pragma once

#include <memory>

#include "MTLGraphicsDevice.h"
#include "MTLResidencyManager.h"
#include "Metal/MTLDevice.hpp"
#include "Private/RHI/RenderHardwareInterface.h"

namespace kgx::RHI
{
class MTLRenderHardwareInterface : public RenderHardwareInterface
{
public:
	MTLRenderHardwareInterface();
	~MTLRenderHardwareInterface() override;

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
		struct SDL_Window* window,
		unsigned int width,
		unsigned int height,
		unsigned int frameCount) const override;

	[[nodiscard]]
	std::unique_ptr<RHIFence> createFence() const override;

	[[nodiscard]]
	std::unique_ptr<RHIShader> createShader(const CompiledShader& compiledShader, RHIShader::ShaderType type) const override;

	[[nodiscard]]
	std::unique_ptr<RHICommandAllocator> createCommandAllocator() const override;

	[[nodiscard]]
	std::unique_ptr<RHIGraphicsCommandList> createGraphicsCommandList(RHIGraphicsPipelineState *pipelineState) const override;

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

	[[nodiscard]]
	MTLResidencyManager* getMTLResidencyManager() const { return mResidencyManager.get(); }

private:
	NS::AutoreleasePool* mAutoReleasePool;
	std::unique_ptr<MTLGraphicsDevice> mGraphicsDevice;
	std::unique_ptr<MTLResidencyManager> mResidencyManager;
};

inline MTLRenderHardwareInterface* getMTLRHI()
{
	return static_cast<MTLRenderHardwareInterface*>(gPlatformRHI.get());
}
}
