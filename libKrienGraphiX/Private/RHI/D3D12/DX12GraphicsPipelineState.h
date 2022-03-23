
#pragma once

#include "Private/RHI/RHIGraphicsPipelineState.h"

#include <d3d12.h>
#include <wrl\client.h>

namespace kgx::RHI
{
class DX12GraphicsPipelineState final : public RHIGraphicsPipelineState
{
    public:
        DX12GraphicsPipelineState();
        ~DX12GraphicsPipelineState() override = default;

        bool construct(RHIGraphicsDevice *device) override;

        [[nodiscard]]
        ID3D12PipelineState * getPipelineState() const;
        
    private:
        Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
};
}