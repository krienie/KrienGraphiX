
#pragma once

#include "Private/RHI/RHIGraphicsCommandList.h"

#include <d3d12.h>
#include <wrl\client.h>

namespace kgx::RHI
{
class DX12GraphicsCommandList final : public RHIGraphicsCommandList
{
    public:
        DX12GraphicsCommandList();
        ~DX12GraphicsCommandList() override = default;

        bool init(RHIGraphicsDevice *device, RHIGraphicsPipelineState * initialState = nullptr) override;
        void close() override;

        [[nodiscard]]
        ID3D12CommandList * getCommandList() const;
        [[nodiscard]]
        ID3D12CommandAllocator * getCommandAllocator() const;

        void clearDepthStencilView(RHIDepthStencilBuffer *dsv /*, clearFlags, depth, stencil*/) override;
        void clearRenderTargetView(/*RHIRenderTargetView *rtv, const float colorRGBA[4]*/) override;

    private:
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
};
}