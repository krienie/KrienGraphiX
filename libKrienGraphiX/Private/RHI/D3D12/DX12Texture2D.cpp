
#include "DX12Texture2D.h"

#include "DX12GraphicsDevice.h"

#include <utility>

#include "d3dx12Residency.h"

using Microsoft::WRL::ComPtr;

namespace kgx::RHI
{
//TODO(KL): Create constructor for placed texture resource
//TODO(KL): Create Texture2D descriptor to reduce the amount of parameters

DX12Texture2D::DX12Texture2D(ComPtr<ID3D12Resource> textureResource, ComPtr<ID3D12DescriptorHeap> descriptorHeap, int heapOffset, D3D12_RESOURCE_STATES initialState,
                                unsigned int width, unsigned int height, unsigned int numMips, unsigned int numSamples, PixelFormat format)
    : RHITexture2D(width, height, numMips, numSamples, format), DX12Resource(textureResource, nullptr, initialState),
        mTextureResource(std::move(textureResource)), mDescriptorHeap(std::move(descriptorHeap)), mDescriptorHeapOffset(heapOffset)
{
}
}