
#pragma once

#include <memory>

#include "MTLGraphicsDevice.h"
#include "MTLResidencyManager.h"
#include "Metal/MTLDevice.hpp"
#include "Private/RHI/RenderHardwareInterface.h"
#include "Private/RHI/RHIGraphicsPipelineState.h"
#include "Private/RHI/RHIResourceView.h"

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

	[[nodiscard]]
	std::unique_ptr<RHIPlatform> createPlatform() const override;

	[[nodiscard]]
	std::unique_ptr<RHISwapChain> createSwapChain(
		struct SDL_Window* window,
		unsigned int width,
		unsigned int height,
		unsigned int frameCount) const override;

	[[nodiscard]]
	std::unique_ptr<RHIFence> createFence(RHIPlatform& platform) const override;

	[[nodiscard]]
	std::unique_ptr<RHIShader> createShader(const CompiledShader& compiledShader, RHIShader::ShaderType type) const override;

	[[nodiscard]]
	std::unique_ptr<RHITexture2D> createDepthStencilBuffer(const RHITexture2DDescriptor& descriptor) const override;

	[[nodiscard]]
	RHITextureHandle createTexture2D(const RHITexture2DDescriptor& descriptor) const override;

	[[nodiscard]]
	std::shared_ptr<RHIResourceView> createResourceView(RHIResourceView::Type type, const std::shared_ptr<RHIViewableResource>& viewedResource, bool isShaderVisible) const override;

	[[nodiscard]]
	std::unique_ptr<RHIGraphicsPipelineState> createGraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc) const override;

	[[nodiscard]]
	std::unique_ptr<RHIBuffer> createBuffer(RHIRenderContext* renderContext, const RHIBufferDescriptor& descriptor) const override;

	[[nodiscard]]
	MTLGraphicsDevice* getMTLDevice() const { return mGraphicsDevice.get(); }

private:
	std::unique_ptr<MTLGraphicsDevice> mGraphicsDevice;
};

inline MTLRenderHardwareInterface* getMTLRHI()
{
	return static_cast<MTLRenderHardwareInterface*>(gPlatformRHI.get());
}
}
