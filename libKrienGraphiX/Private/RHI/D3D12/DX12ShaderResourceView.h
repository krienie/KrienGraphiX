
#pragma once

#include "Private/RHI/RHIShaderResourceView.h"

#include <d3d12.h>
#include <wrl\client.h>

namespace kgx::RHI
{
class DX12ShaderResourceView : public RHIShaderResourceView
{
    public:
        DX12ShaderResourceView(Microsoft::WRL::ComPtr<ID3D12Resource> resource, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap);
        ~DX12ShaderResourceView() override = default;

        bool init(RHIGraphicsDevice *device) override;

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
};
}