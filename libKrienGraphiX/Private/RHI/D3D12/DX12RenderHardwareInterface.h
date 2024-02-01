
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

    void beginFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget) override;
    void endFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget) override;

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
    std::unique_ptr<RHIShader> createShader(RHIGraphicsDevice* graphicsDevice, RHIGraphicsCommandList* commandList, const CompiledShader& compiledShader, RHIShader::ShaderType type) override;

    [[nodiscard]]
    std::unique_ptr<RHIGraphicsCommandList> createGraphicsCommandList(RHIGraphicsDevice *graphicsDevice, RHICommandQueue* commandQueue, RHIGraphicsPipelineState *pipelineState) override;

    [[nodiscard]]
    std::unique_ptr<RHITexture2D> createDepthStencilBuffer(RHIGraphicsDevice* graphicsDevice, RHITexture2DDescriptor descriptor) override;

    [[nodiscard]]
    std::shared_ptr<RHIResourceView> createResourceView(RHIResourceView::ViewType type, const std::shared_ptr<RHIViewableResource>& viewedResource, bool isShaderVisible) override;

    [[nodiscard]]
    std::unique_ptr<RHIGraphicsPipelineState> createGraphicsPipelineState(RHIGraphicsDevice* graphicsDevice, const RHIGraphicsPipelineStateDescriptor& desc) override;

    [[nodiscard]]
    std::unique_ptr<RHIBuffer> createBuffer(RHIGraphicsDevice* graphicsDevice, RHIGraphicsCommandList* commandList, const RHIBufferDescriptor& descriptor) override;
};
}
