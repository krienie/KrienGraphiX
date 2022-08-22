
#pragma once

#include "RHICommandQueue.h"
#include "RHIConstantBuffer.h"
#include "RHIDepthStencilBuffer.h"
#include "RHIDescriptors.h"
#include "RHIGraphicsCommandList.h"
#include "RHIGraphicsDevice.h"
#include "RHIGraphicsPipelineState.h"
#include "RHIShaderProgram.h"
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

        virtual void beginFrame(RHICommandQueue* commandQueue) = 0;
        virtual void endFrame() = 0;

        [[nodiscard]]
        virtual std::unique_ptr<RHIGraphicsDevice>  createGraphicsDevice() = 0;

        [[nodiscard]]
        virtual std::unique_ptr<RHICommandQueue> createCommandQueue(RHIGraphicsDevice *graphicsDevice) = 0;

        [[nodiscard]]
        virtual std::unique_ptr<RHISwapChain> createSwapChain(
            RHIGraphicsDevice * device,
            RHICommandQueue * commandQueue,
            WinHandle windowHandle,
            unsigned int width,
            unsigned int height,
            unsigned int frameCount) = 0;

        [[nodiscard]]
        virtual std::unique_ptr<RHIShaderProgram> createShaderProgram() = 0;

        [[nodiscard]]
        virtual std::unique_ptr<RHIGraphicsPipelineState> createGraphicsPipelineState(RHIGraphicsDevice *graphicsDevice) = 0;

        [[nodiscard]]
        virtual std::unique_ptr<RHIGraphicsCommandList> createGraphicsCommandList(RHIGraphicsDevice *graphicsDevice, RHICommandQueue* commandQueue, RHIGraphicsPipelineState *pipelineState) = 0;

        [[nodiscard]]
        virtual std::unique_ptr<RHIDepthStencilBuffer> createDepthStencilBuffer(RHIGraphicsDevice *graphicsDevice, unsigned int width, unsigned int height) = 0;

        [[nodiscard]]
        virtual std::unique_ptr<RHIConstantBuffer> createConstantBuffer(RHIGraphicsDevice *graphicsDevice, RHIConstantBufferDescriptor cbDescriptor) = 0;

        //[[nodiscard]]
        //virtual std::shared_ptr<RHITexture2D> createTexture2D(RHIGraphicsDevice *graphicsDevice, unsigned int width, unsigned int height) = 0;
};
}
