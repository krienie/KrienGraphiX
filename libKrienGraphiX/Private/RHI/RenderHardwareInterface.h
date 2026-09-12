
#pragma once

#include <memory>

#include "RHIBuffer.h"
#include "RHIFence.h"
#include "RHIGraphicsPipelineState.h"
#include "RHIPlatform.h"
#include "RHIResourceView.h"
#include "RHIShader.h"
#include "RHISwapChain.h"
#include "RHITextureHandle.h"
#include "RHIVertexLayout.h"

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

	[[nodiscard]]
	virtual std::unique_ptr<RHIPlatform> createPlatform() const = 0;

	[[nodiscard]]
	virtual std::unique_ptr<RHISwapChain> createSwapChain(
		struct SDL_Window* window,
		unsigned int width,
		unsigned int height,
		unsigned int frameCount) const = 0;

	[[nodiscard]]
	virtual std::unique_ptr<RHIFence> createFence(RHIPlatform& platform) const = 0;

	[[nodiscard]]
	virtual std::unique_ptr<RHIShader> createShader(const CompiledShader& compiledShader, RHIShader::ShaderType type) const = 0;

	//TODO(KL): Replace with createTexture()
	[[nodiscard]]
	virtual std::unique_ptr<RHITexture2D> createDepthStencilBuffer(const RHITexture2DDescriptor& descriptor) const = 0;

	[[nodiscard]]
	virtual RHITextureHandle createTexture2D(const RHITexture2DDescriptor& descriptor) const = 0;

	[[nodiscard]]
	virtual std::shared_ptr<RHIResourceView> createResourceView(RHIResourceView::Type type, const std::shared_ptr<RHIViewableResource>& viewedResource, bool isShaderVisible) const = 0;

	[[nodiscard]]
	virtual std::unique_ptr<RHIGraphicsPipelineState> createGraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc) const = 0;

	[[nodiscard]]
	virtual std::unique_ptr<RHIBuffer> createBuffer(RHIRenderContext* renderContext, const RHIBufferDescriptor& descriptor) const = 0;
};

inline std::unique_ptr<RenderHardwareInterface> gPlatformRHI = nullptr;
}
