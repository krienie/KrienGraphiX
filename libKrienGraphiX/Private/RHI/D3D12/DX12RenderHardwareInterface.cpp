
#include "DX12RenderHardwareInterface.h"

#include "DX12GraphicsDevice.h"
#include "DX12CommandQueue.h"
#include "DX12DepthStencilBuffer.h"
#include "DX12GraphicsCommandList.h"
#include "DX12GraphicsPipelineState.h"
#include "DX12ShaderProgram.h"
#include "DX12SwapChain.h"

namespace kgx::RHI
{
//TODO(KL): log errors when any of these inits fail

std::unique_ptr<RHIGraphicsDevice> DX12RenderHardwareInterface::createGraphicsDevice()
{
    auto graphicsDevice = std::make_unique<DX12GraphicsDevice>();
    if (!graphicsDevice->init())
    {
        return nullptr;
    }

    return std::move(graphicsDevice);
}

std::unique_ptr<RHICommandQueue> DX12RenderHardwareInterface::createCommandQueue(RHIGraphicsDevice * graphicsDevice)
{
    auto commandQueue = std::make_unique<DX12CommandQueue>();
    if (!commandQueue->init(graphicsDevice))
    {
        return nullptr;
    }

    return std::move(commandQueue);
}

std::unique_ptr<RHISwapChain> DX12RenderHardwareInterface::createSwapChain(
    RHIGraphicsDevice * device,
    RHICommandQueue * commandQueue,
    WinHandle windowHandle,
    unsigned int width,
    unsigned int height,
    unsigned int frameCount)
{
    auto swapChain = std::make_unique<DX12SwapChain>(width, height);
    if (!swapChain->init(device, commandQueue, windowHandle, frameCount))
    {
        return nullptr;
    }

    return std::move(swapChain);
}

std::unique_ptr<RHIShaderProgram> DX12RenderHardwareInterface::createShaderProgram()
{
    return std::make_unique<DX12ShaderProgram>();
}



//TODO(KL): Temporary maybe_unused
std::unique_ptr<RHIGraphicsPipelineState> DX12RenderHardwareInterface::createGraphicsPipelineState([[maybe_unused]] RHIGraphicsDevice * graphicsDevice)
{
    auto pipelineState = std::make_unique<DX12GraphicsPipelineState>();
    //if (!pipelineState->init(graphicsDevice))
    //{
    //    return nullptr;
    //}

    return std::move(pipelineState);
}

std::unique_ptr<RHIGraphicsCommandList> DX12RenderHardwareInterface::createGraphicsCommandList(RHIGraphicsDevice* graphicsDevice, RHICommandQueue* commandQueue, RHIGraphicsPipelineState* pipelineState)
{
    auto graphicsCommandList = std::make_unique<DX12GraphicsCommandList>();
    if (graphicsCommandList->init(graphicsDevice, commandQueue, pipelineState))
    {
        return nullptr;
    }

    return std::move(graphicsCommandList);
}

std::unique_ptr<RHIDepthStencilBuffer> DX12RenderHardwareInterface::createDepthStencilBuffer(RHIGraphicsDevice * graphicsDevice, unsigned int width, unsigned int height)
{
    auto depthStencilBuffer = std::make_unique<DX12DepthStencilBuffer>(width, height);
    if (!depthStencilBuffer->init(graphicsDevice))
    {
        return nullptr;
    }

    return std::move(depthStencilBuffer);
}

//TODO(KL): Temporary maybe_unused
std::unique_ptr<RHIConstantBuffer> DX12RenderHardwareInterface::createConstantBuffer([[maybe_unused]] RHIGraphicsDevice * graphicsDevice, [[maybe_unused]] RHIConstantBufferDescriptor cbDescriptor)
{
    return std::unique_ptr<RHIConstantBuffer>();
}
}
