
#include "DX12ShaderResourceView.h"

#include "DX12GraphicsDevice.h"

#include <utility>

#include "d3dx12residency.h"

using Microsoft::WRL::ComPtr;

namespace kgx::RHI
{
DX12Texture2D::DX12Texture2D(ComPtr<ID3D12Resource> textureResource, ComPtr<ID3D12DescriptorHeap> descriptorHeap)
    : RHITexture2D(), mTextureResource(std::move(textureResource)), mDescriptorHeap(std::move(descriptorHeap))
{
}

bool DX12Texture2D::init(RHIGraphicsDevice *device)
{
    auto * dxDevice = dynamic_cast<DX12GraphicsDevice*>(device);
    if (dxDevice == nullptr)
    {
        // This should never happen
        return false;
    }


    return SUCCEEDED(res);
}

}