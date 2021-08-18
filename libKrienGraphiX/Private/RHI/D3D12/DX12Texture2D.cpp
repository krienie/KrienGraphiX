
#include "DX12Texture2D.h"

#include "DX12GraphicsDevice.h"

#include <utility>

#include "d3dx12Residency.h"

using Microsoft::WRL::ComPtr;

namespace kgx::RHI
{
DX12Texture2D::DX12Texture2D(ComPtr<ID3D12Resource> textureResource, ComPtr<ID3D12DescriptorHeap> descriptorHeap,
                                unsigned int width, unsigned int height, unsigned int numMips, unsigned int numSamples, PixelFormat format)
    : RHITexture2D(width, height, numMips, numSamples, format),
        mTextureResource(std::move(textureResource)), mDescriptorHeap(std::move(descriptorHeap))
{
}
}