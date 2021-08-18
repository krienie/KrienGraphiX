
#pragma once

#include "Private/RHI/RHISwapChain.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl\client.h>

#include <vector>

namespace kgx::RHI
{
class DX12Texture2D;

class DX12SwapChain : public RHISwapChain
{
    public:
        DX12SwapChain(UINT width, UINT height);
        ~DX12SwapChain() override = default;

        bool init(RHIGraphicsDevice *device, RHICommandQueue * commandQueue, WinHandle windowHandle, unsigned int frameCount) override;

        [[nodiscard]]
        unsigned int getFrameIndex() const override;

        [[nodiscard]]
        std::weak_ptr<RHITexture2D> getBuffer(unsigned int idx) const override;

    private:
        Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;
        //Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
        //std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> mRenderTargets;
        std::vector<std::shared_ptr<DX12Texture2D>> mRenderTargets;

        UINT mWidth;
        UINT mHeight;
};
}
