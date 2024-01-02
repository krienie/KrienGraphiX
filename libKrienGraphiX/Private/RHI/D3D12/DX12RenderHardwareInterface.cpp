
#include "DX12RenderHardwareInterface.h"

#include "DX12GraphicsDevice.h"
#include "DX12CommandQueue.h"
#include "DX12GraphicsCommandList.h"
#include "DX12GraphicsPipelineState.h"
#include "DX12Shader.h"
#include "DX12SwapChain.h"

#include <cassert>

#include "DX12Descriptors.h"
#include "DX12Texture2D.h"
#include "Private/Core/RenderCore.h"

namespace kgx::RHI
{
//TODO(KL): log errors when any of these inits fail

void DX12RenderHardwareInterface::beginFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget)
{
    const auto* renderCore = core::RenderCore::get();

    auto* commandQueue = renderCore->getRenderThreadPtr()->getCommandQueuePtr();

    auto* dxCommandQueue = static_cast<DX12CommandQueue*>(commandQueue);
    dxCommandQueue->getNativeCommandAllocator()->Reset();

    commandList->reset(commandQueue);

    auto* dxTexture2D = static_cast<DX12Texture2D*>(renderTarget);
    dxTexture2D->transitionToState(static_cast<DX12GraphicsCommandList*>(commandList), D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void DX12RenderHardwareInterface::endFrame(RHIGraphicsCommandList* commandList, RHITexture2D* renderTarget)
{
    auto* dxTexture2D = static_cast<DX12Texture2D*>(renderTarget);
    dxTexture2D->transitionToState(static_cast<DX12GraphicsCommandList*>(commandList), D3D12_RESOURCE_STATE_PRESENT);

    commandList->close();
}

std::unique_ptr<RHIGraphicsDevice> DX12RenderHardwareInterface::createGraphicsDevice()
{
    auto graphicsDevice = std::make_unique<DX12GraphicsDevice>();
    if (!graphicsDevice->create())
    {
        return nullptr;
    }

    return std::move(graphicsDevice);
}

std::unique_ptr<RHICommandQueue> DX12RenderHardwareInterface::createCommandQueue(RHIGraphicsDevice * graphicsDevice)
{
    auto commandQueue = std::make_unique<DX12CommandQueue>();
    if (!commandQueue->create(graphicsDevice))
    {
        return nullptr;
    }

    return std::move(commandQueue);
}

std::unique_ptr<RHISwapChain> DX12RenderHardwareInterface::createSwapChain(
    RHIGraphicsDevice* device,
    RHICommandQueue* commandQueue,
    WinHandle windowHandle,
    unsigned int width,
    unsigned int height,
    unsigned int frameCount)
{
    auto swapChain = std::make_unique<DX12SwapChain>(width, height);
    if (!swapChain->create(device, commandQueue, windowHandle, frameCount, RHIPixelFormat::R10G10B10A2_unorm))
    {
        return nullptr;
    }

    return std::move(swapChain);
}

std::unique_ptr<RHIShader> DX12RenderHardwareInterface::createShader(RHIGraphicsDevice* graphicsDevice, const CompiledShader& compiledShader, RHIShader::ShaderType type)
{
    auto newShader = std::make_unique<DX12Shader>();
    if (!newShader->create(graphicsDevice, compiledShader, type))
    {
        return nullptr;
    }

    return std::move(newShader);
}

std::unique_ptr<RHIGraphicsCommandList> DX12RenderHardwareInterface::createGraphicsCommandList(RHIGraphicsDevice* graphicsDevice, RHICommandQueue* commandQueue, RHIGraphicsPipelineState* pipelineState)
{
    auto graphicsCommandList = std::make_unique<DX12GraphicsCommandList>();
    if (!graphicsCommandList->create(graphicsDevice, commandQueue, pipelineState))
    {
        return nullptr;
    }

    return std::move(graphicsCommandList);
}

std::unique_ptr<RHITexture2D> DX12RenderHardwareInterface::createDepthStencilBuffer(RHIGraphicsDevice* graphicsDevice, RHITexture2DDescriptor descriptor)
{
    //TODO(KL): Validate pixelformat

    DX12Texture2DDescriptor dx12Desc =
    {
        descriptor,
        nullptr,
        nullptr,
        0,
        D3D12_RESOURCE_STATE_DEPTH_WRITE
    };

    auto* dxDevice = static_cast<DX12GraphicsDevice*>(graphicsDevice);
    auto depthStencilBuffer = std::make_unique<DX12Texture2D>(dxDevice, dx12Desc);

    return std::move(depthStencilBuffer);
}

std::shared_ptr<RHIResourceView> DX12RenderHardwareInterface::createResourceView(RHIResourceView::ViewType type, const std::shared_ptr<RHIViewableResource>& viewedResource, bool isShaderVisible)
{
    return std::make_shared<DX12ResourceView>(type, viewedResource, isShaderVisible);
}

std::unique_ptr<RHIGraphicsPipelineState> DX12RenderHardwareInterface::createGraphicsPipelineState(RHIGraphicsDevice* graphicsDevice, const RHIGraphicsPipelineStateDescriptor& desc)
{
    auto* dxDevice = static_cast<DX12GraphicsDevice*>(graphicsDevice);
    
    auto graphicsPipelineState = std::make_unique<DX12GraphicsPipelineState>(desc);
    if (!graphicsPipelineState->create(dxDevice))
    {
        return nullptr;
    }

    return std::move(graphicsPipelineState);
}
}
