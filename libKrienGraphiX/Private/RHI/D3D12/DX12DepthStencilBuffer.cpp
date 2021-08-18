
#include "DX12DepthStencilBuffer.h"

#include "DX12GraphicsDevice.h"

#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

namespace kgx::RHI
{
DX12DepthStencilBuffer::DX12DepthStencilBuffer(UINT width, UINT height)
    : RHIDepthStencilBuffer(), mDsvDescriptorHeap(nullptr), mDepthStencil(nullptr),
        mWidth(width), mHeight(height)
{
}

bool DX12DepthStencilBuffer::init(RHIGraphicsDevice *device)
{
    auto * dxDevice = dynamic_cast<DX12GraphicsDevice*>(device);
    if (dxDevice == nullptr)
    {
        // This should never happen
        return false;
    }

    ID3D12Device * nativeDevice = dxDevice->getNativeDevice();

    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    // Create the actual depth stencil resource, including a heap for it to live in
    HRESULT res = nativeDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, mWidth, mHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthOptimizedClearValue,
        IID_PPV_ARGS(&mDepthStencil));

    if (FAILED(res))
    {
        return false;
    }

    // Describe and create a depth stencil view (DSV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    res = nativeDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvDescriptorHeap));
    if (FAILED(res))
    {
        return false;
    }

    // Create a depth stencil view in the previously 
    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
        depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

    nativeDevice->CreateDepthStencilView(mDepthStencil.Get(), &depthStencilDesc, mDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    return SUCCEEDED(res);
}

ID3D12DescriptorHeap * DX12DepthStencilBuffer::getDescriptorHeap() const
{
    return mDsvDescriptorHeap.Get();
}

ID3D12Resource * DX12DepthStencilBuffer::getResource() const
{
    return mDepthStencil.Get();
}

}