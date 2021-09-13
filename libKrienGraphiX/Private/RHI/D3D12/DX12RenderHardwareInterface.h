
#pragma once

#include "Private/RHI/RenderHardwareInterface.h"

#include <string>

namespace kgx::RHI
{
class DX12RenderHardwareInterface : public RenderHardwareInterface
{
    public:
        DX12RenderHardwareInterface() = default;
        ~DX12RenderHardwareInterface() override = default;

        DX12RenderHardwareInterface(const DX12RenderHardwareInterface&) noexcept            = delete;
        DX12RenderHardwareInterface& operator=(const DX12RenderHardwareInterface&) noexcept = delete;
        DX12RenderHardwareInterface(DX12RenderHardwareInterface&&) noexcept                 = delete;
        DX12RenderHardwareInterface& operator=(DX12RenderHardwareInterface&&) noexcept      = delete;

        [[nodiscard]]
        std::unique_ptr<RHIGraphicsDevice> createGraphicsDevice() override;

        [[nodiscard]]
        std::unique_ptr<RHICommandQueue> createCommandQueue(RHIGraphicsDevice *graphicsDevice) override;

        [[nodiscard]]
        std::unique_ptr<RHISwapChain> createSwapChain(
            RHIGraphicsDevice * device,
            RHICommandQueue * commandQueue,
            WinHandle windowHandle,
            unsigned int width,
            unsigned int height,
            unsigned int frameCount) override;

        [[nodiscard]]
        std::unique_ptr<RHIShaderProgram> createShaderProgram() override;

        [[nodiscard]]
        std::unique_ptr<RHIGraphicsPipelineState> createGraphicsPipelineState(RHIGraphicsDevice *graphicsDevice) override;

        [[nodiscard]]
        std::unique_ptr<RHIGraphicsCommandList> createGraphicsCommandList(RHIGraphicsDevice *graphicsDevice, RHIGraphicsPipelineState *pipelineState) override;

        [[nodiscard]]
        std::unique_ptr<RHIDepthStencilBuffer> createDepthStencilBuffer(RHIGraphicsDevice *graphicsDevice, unsigned int width, unsigned int height) override;

        [[nodiscard]]
        std::unique_ptr<RHIConstantBuffer> createConstantBuffer(RHIGraphicsDevice *graphicsDevice, RHIConstantBufferDescriptor cbDescriptor) override;
};
}
