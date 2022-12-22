
#include "DX12DepthStencilBuffer.h"

#include "DX12GraphicsDevice.h"
#include "DX12Descriptors.h"

#include <cassert>

using Microsoft::WRL::ComPtr;

namespace kgx::RHI
{
DX12DepthStencilBuffer::DX12DepthStencilBuffer(DX12GraphicsDevice* dxDevice, const RHITexture2DDescriptor& descriptor)
    : RHIDepthStencilBuffer(descriptor)
{
    assert(dxDevice);
    
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
