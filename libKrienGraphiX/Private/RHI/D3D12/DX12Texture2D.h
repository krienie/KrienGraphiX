
#pragma once

#include "Private/RHI/RHITexture2D.h"

#include <d3d12.h>
#include <wrl\client.h>

#include "DX12Resource.h"

namespace kgx::RHI
{
class DX12Texture2D : public RHITexture2D, public DX12Resource
{
public:
    DX12Texture2D(Microsoft::WRL::ComPtr<ID3D12Resource> textureResource, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, int heapOffset,
                    D3D12_RESOURCE_STATES initialState, unsigned int width, unsigned int height, unsigned int numMips, unsigned int numSamples, PixelFormat format);
    ~DX12Texture2D() override = default;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> mTextureResource;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
    int mDescriptorHeapOffset;
};
}
