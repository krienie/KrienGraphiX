
#pragma once

#include "RHICommandQueue.h"
#include "RHIConstantBuffer.h"
#include "RHIDepthStencilBuffer.h"
#include "RHIGraphicsCommandList.h"
#include "RHIGraphicsDevice.h"
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
    virtual std::unique_ptr<RHIGraphicsDevice>  createGraphicsDevice() = 0;

    [[nodiscard]]
    virtual std::unique_ptr<RHICommandQueue> createCommandQueue(RHIGraphicsDevice* graphicsDevice) = 0;

    [[nodiscard]]
    virtual std::unique_ptr<RHISwapChain> createSwapChain(
        RHIGraphicsDevice* device,
        RHICommandQueue* commandQueue,
        WinHandle windowHandle,
        unsigned int width,
        unsigned int height,
        unsigned int frameCount) = 0;

    [[nodiscard]]
    virtual std::unique_ptr<RHIShader> createShader(RHIGraphicsDevice* graphicsDevice, const CompiledShader& compiledShader, RHIShader::ShaderType type) = 0;
    
    [[nodiscard]]
    virtual std::unique_ptr<RHIGraphicsCommandList> createGraphicsCommandList(RHIGraphicsDevice* graphicsDevice, RHICommandQueue* commandQueue, RHIGraphicsPipelineState *pipelineState) = 0;

    [[nodiscard]]
    virtual std::unique_ptr<RHIDepthStencilBuffer> createDepthStencilBuffer(RHIGraphicsDevice* graphicsDevice, RHITexture2DDescriptor descriptor) = 0;

    [[nodiscard]]
    virtual std::unique_ptr<RHIGraphicsPipelineState> createGraphicsPipelineState(RHIGraphicsDevice* graphicsDevice, const RHIGraphicsPipelineStateDescriptor& desc) = 0;
};

inline std::unique_ptr<RenderHardwareInterface> PlatformRHI = nullptr;
}
