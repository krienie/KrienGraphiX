
#pragma once

#include <d3d12.h>
#include <memory>
#include <wrl\client.h>

#include "Private/RHI/RHIResourceView.h"

namespace kgx::RHI
{
class DX12Resource;
class DX12GraphicsCommandList;

class DX12ResourceView : public RHIResourceView
{
public:
    //enum class Dimension
    //{
    //    Buffer,
    //    Texture1D,
    //    Texture1DArray,
    //    Texture2D,
    //    Texture2DArray,
    //    Texture2DMS,
    //    Texture2DMSArray,
    //    Texture3D,
    //    TextureCube,
    //    TextureCubeArray,
    //    RaytracingAccelerationStructure
    //};

    //TODO(KL): Add customization options for typeless resources
    DX12ResourceView(ViewType type, const std::shared_ptr<RHIResource>& viewedResource);
    DX12ResourceView(ViewType type, const std::shared_ptr<RHIResource>& viewedResource, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, int descriptorOffset);

    DX12ResourceView(const DX12ResourceView& rhs) = default;
    DX12ResourceView(DX12ResourceView&& rhs) = default;
    DX12ResourceView& operator=(const DX12ResourceView& rhs) = default;
    DX12ResourceView& operator=(DX12ResourceView&& rhs) = default;
    
    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getViewHandle() const;

private:
    void createView();

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;

    int mDescriptorOffset = 0;
};
}
