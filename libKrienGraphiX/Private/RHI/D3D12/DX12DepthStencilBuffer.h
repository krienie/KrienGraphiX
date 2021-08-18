
#pragma once

#include "Private/RHI/RHIDepthStencilBuffer.h"

#include <D3D12.h>
#include <wrl\client.h>

namespace kgx::RHI
{
class DX12DepthStencilBuffer : public RHIDepthStencilBuffer
{
    public:
        DX12DepthStencilBuffer(UINT width, UINT height);
        ~DX12DepthStencilBuffer() override = default;

        bool init(RHIGraphicsDevice *device) override;

        ID3D12DescriptorHeap * getDescriptorHeap() const;
        ID3D12Resource * getResource() const;

    private:
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvDescriptorHeap;
        Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencil;

        UINT mWidth;
        UINT mHeight;
};
}