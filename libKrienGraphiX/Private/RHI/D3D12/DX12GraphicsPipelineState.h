
#pragma once

#include "Private/RHI/RHIGraphicsPipelineState.h"

#include <d3d12.h>
#include <wrl\client.h>

namespace kgx::RHI
{
class DX12GraphicsPipelineState final : public RHIGraphicsPipelineState
{
public:
    DX12GraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc);
    ~DX12GraphicsPipelineState() override = default;

    bool create(RHIGraphicsDevice* device) override;

    [[nodiscard]]
    ID3D12PipelineState* getPSO() const;
    
private:
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
};
}