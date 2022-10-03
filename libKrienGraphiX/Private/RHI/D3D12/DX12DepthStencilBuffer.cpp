
#include "DX12DepthStencilBuffer.h"

#include "DX12GraphicsDevice.h"
#include "DX12Descriptors.h"
#include "DX12PixelFormat.h"

#include <cassert>

using Microsoft::WRL::ComPtr;

namespace kgx::RHI
{
DX12DepthStencilBuffer::DX12DepthStencilBuffer(DX12GraphicsDevice* dxDevice, const RHITexture2DDescriptor& descriptor)
    : RHIDepthStencilBuffer(descriptor)
{
    assert(dxDevice);
    
    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = toDxgiPixelFormat(pixelFormat());
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    DX12Texture2DDescriptor desc =
        {
            descriptor,
            nullptr,
            nullptr,
            0,
            D3D12_RESOURCE_STATE_DEPTH_WRITE
        };

    mTexture = std::make_shared<DX12Texture2D>(dxDevice, desc);
    addResourceView(std::make_shared<DX12ResourceView>(DX12ResourceView::ViewType::DSV, mTexture));

    // Create the actual depth stencil resource, including a heap for it to live in
    //HRESULT res = nativeDevice->CreateCommittedResource(
    //    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
    //    D3D12_HEAP_FLAG_NONE,
    //    &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width(), height(), 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
    //    D3D12_RESOURCE_STATE_DEPTH_WRITE,
    //    &depthOptimizedClearValue,
    //    IID_PPV_ARGS(&mDepthStencil));

    //if (FAILED(res))
    //{
    //    return false;
    //}

    //// Describe and create a depth stencil view (DSV) descriptor heap.
    //D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    //dsvHeapDesc.NumDescriptors = 1;
    //dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    //dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    //
    //res = nativeDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvDescriptorHeap));
    //if (FAILED(res))
    //{
    //    return false;
    //}
    //
    //// Create a depth stencil view in the previously 
    //D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
    //    depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
    //    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    //    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
    //
    //nativeDevice->CreateDepthStencilView(mDepthStencil.Get(), &depthStencilDesc, mDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

D3D12_RESOURCE_STATES DX12DepthStencilBuffer::getCurrentState() const
{
    assert(mTexture);
    return mTexture->getCurrentState();
}

ComPtr<ID3D12Resource> DX12DepthStencilBuffer::getResource() const
{
    assert(mTexture);
    return mTexture->getResource();
}

ComPtr<ID3D12Heap> DX12DepthStencilBuffer::getHeap() const
{
    assert(mTexture);
    return mTexture->getHeap();
}

RHIResourceView* DX12DepthStencilBuffer::getDepthStencilView() const
{
    assert(mTexture);
    return mTexture->getResourceViewByType(RHIResourceView::DSV).get();
}
}
