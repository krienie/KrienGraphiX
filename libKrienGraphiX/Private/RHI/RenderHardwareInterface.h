
#pragma once

#include "Private/RHI/RHICommandQueue.h"
#include "Private/RHI/RHIDescriptors.h"
#include "Private/RHI/RHIGraphicsDevice.h"
#include "Private/RHI/RHISwapChain.h"

#include <memory>
#include <string>

namespace kgx::RHI
{
class RHIConstantBuffer;
class RHIDepthStencilBuffer;
class RHIGraphicsCommandList;
class RHIGraphicsPipelineState;
class RHIShaderProgram;
class RHITexture2D;

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
        virtual std::unique_ptr<RHIGraphicsCommandList> createGraphicsCommandList(RHIGraphicsDevice *graphicsDevice, RHIGraphicsPipelineState *pipelineState) = 0;

        [[nodiscard]]
        virtual std::unique_ptr<RHIDepthStencilBuffer> createDepthStencilBuffer(RHIGraphicsDevice *graphicsDevice, unsigned int width, unsigned int height) = 0;

        [[nodiscard]]
        virtual std::unique_ptr<RHIConstantBuffer> createConstantBuffer(RHIGraphicsDevice *graphicsDevice, RHIConstantBufferDescriptor cbDescriptor) = 0;

        //[[nodiscard]]
        //virtual std::shared_ptr<RHITexture2D> createTexture2D(RHIGraphicsDevice *graphicsDevice, unsigned int width, unsigned int height) = 0;
};
}
