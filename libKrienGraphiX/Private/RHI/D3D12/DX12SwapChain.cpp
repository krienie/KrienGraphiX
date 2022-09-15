
#include "DX12SwapChain.h"

#include "DX12CommandQueue.h"
#include "DX12GraphicsDevice.h"
#include "DX12Texture2D.h"
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

namespace kgx::RHI
{
DX12SwapChain::DX12SwapChain(UINT width, UINT height)
    : RHISwapChain(), mSwapChain(nullptr), mRtvHeap(nullptr),
        mWidth(width), mHeight(height)
{
}

bool DX12SwapChain::init(RHIGraphicsDevice* device, RHICommandQueue* commandQueue, WinHandle windowHandle, unsigned int bufferCount)
{
    auto* dxDevice = dynamic_cast<DX12GraphicsDevice*>(device);
    if (dxDevice == nullptr)
    {
        // This should never happen
        return false;
    }

    auto * nativeFactory = dxDevice->getNativeFactory();

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = bufferCount;
    swapChainDesc.Width = mWidth;
    swapChainDesc.Height = mHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    auto * dxCommandQueue = dynamic_cast<DX12CommandQueue*>(commandQueue);
    if (dxCommandQueue == nullptr)
    {
        // This should never happen
        return false;
    }

    auto * nativeCommandQueue = dxCommandQueue->getNativeCommandQueue();

    ComPtr<IDXGISwapChain1> swapChain;

    HRESULT res = nativeFactory->CreateSwapChainForHwnd(
        nativeCommandQueue,
        reinterpret_cast<HWND>(windowHandle),
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    );

    if (FAILED(res))
    {
        return false;
    }

    swapChain.As(&mSwapChain);

    // Turn off fullscreen transitions
    res = nativeFactory->MakeWindowAssociation(reinterpret_cast<HWND>(windowHandle), DXGI_MWA_NO_ALT_ENTER);
    if ((FAILED(res)))
    {
        return false;
    }

    auto * nativeDevice = dxDevice->getNativeDevice();

    // Create descriptor heap for the backbuffer RTV's
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = bufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    res = nativeDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap));
    if ((FAILED(res)))
    {
        return false;
    }

    //TODO(KL): maybe store this descriptor size in RHITexture2D...
    const UINT rtvDescriptorSize = nativeDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    std::vector<ComPtr<ID3D12Resource>> buffers(bufferCount);

    // Create render target resources
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0u; i < bufferCount; ++i)
    {
        res = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&buffers[i]));
        if ((FAILED(res)))
        {
            return false;
        }

        nativeDevice->CreateRenderTargetView(buffers[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, rtvDescriptorSize);

        mBuffers.push_back(std::make_shared<DX12Texture2D>(buffers[i], mRtvHeap, i, D3D12_RESOURCE_STATE_PRESENT, mWidth, mHeight, 1u, 1u, PixelFormat::R8G8B8A8));
    }

    return SUCCEEDED(res);
}

std::shared_ptr<RHITexture2D> DX12SwapChain::getCurrentBuffer() const
{
    return mBuffers[mSwapChain->GetCurrentBackBufferIndex()];
}

void DX12SwapChain::present()
{
    mSwapChain->Present(0, 0);
}

}