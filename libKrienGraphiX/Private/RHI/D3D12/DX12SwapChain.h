
#pragma once

#include "Private/RHI/RHISwapChain.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl\client.h>

#include <vector>

namespace kgx::RHI
{
class DX12ResourceView;
class DX12Texture2D;

class DX12SwapChain : public RHISwapChain
{
    public:
        DX12SwapChain(UINT width, UINT height);
        ~DX12SwapChain() override = default;

        bool create(RHIGraphicsDevice* device, RHICommandQueue* commandQueue, WinHandle windowHandle, unsigned int bufferCount, RHIPixelFormat pixelFormat) override;

        [[nodiscard]] RHIResourceView* getCurrentBufferView() const override;
        
        void present() override;

    private:
        Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
        std::vector<std::shared_ptr<DX12Texture2D>> mBuffers;
        std::vector<std::shared_ptr<DX12ResourceView>> mBufferViews;

        UINT mWidth;
        UINT mHeight;
};
}
