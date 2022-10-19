
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

        [[nodiscard]]
        ID3D12GraphicsCommandList* getCommandList() const { return mCommandList.Get(); }

        bool init(RHIGraphicsDevice* device, RHICommandQueue* commandQueue, RHIGraphicsPipelineState* initialState = nullptr) override;

        void close() override;
        void reset(RHICommandQueue* commandQueue) override;

        void setViewport(const KGXViewport &viewport) override;

        void setRenderTargets(const std::vector<RHIResourceView*>& renderTargetViews, const RHIResourceView* depthStencilView) override;

        void clearDepthStencilView(RHIResourceView* dsv, RHIResourceView::DepthStencilFlags clearFlags, float depth, uint8_t stencil) override;
        void clearRenderTargetView(RHIResourceView* rtv, const float colorRGBA[4]) override;

    private:
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
};
}