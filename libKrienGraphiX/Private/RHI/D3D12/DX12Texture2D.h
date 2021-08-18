
#pragma once

#include "Private/RHI/RHITexture2D.h"

#include <d3d12.h>
#include <wrl\client.h>

namespace kgx::RHI
{
class DX12Texture2D : public RHITexture2D
{
    public:
        //TODO(KL): DX12Texture2D(size, GLenum minMaxFilter, GLenum format, GLenum type, GLint wrapMode, core::Math::Color4f border = {0,0,0,0});
        DX12Texture2D(Microsoft::WRL::ComPtr<ID3D12Resource> textureResource, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,
            unsigned int width, unsigned int height, unsigned int numMips, unsigned int numSamples, PixelFormat format);
        ~DX12Texture2D() override = default;

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> mTextureResource;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
};
}