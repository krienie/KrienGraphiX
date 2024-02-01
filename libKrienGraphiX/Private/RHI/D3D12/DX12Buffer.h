
#pragma once

#include "DX12Resource.h"
#include "Private/RHI/RHIBuffer.h"

namespace kgx::RHI
{
class DX12GraphicsDevice;

class DX12Buffer final : public RHIBuffer, public DX12Resource
{
public:
    DX12Buffer(DX12GraphicsDevice* dxDevice, DX12GraphicsCommandList* commandList, const RHIBufferDescriptor& descriptor);
    ~DX12Buffer() override = default;

    DX12Buffer(DX12Buffer&) = delete;
    DX12Buffer(DX12Buffer&&) noexcept = default;
    DX12Buffer& operator=(DX12Buffer&) = delete;
    DX12Buffer& operator=(DX12Buffer&&) noexcept = default;
    
private:
    void* mapImpl(MapType type) override;
    void unmapImpl() override;
    
    RHIBufferDescriptor mDescriptor;
    Microsoft::WRL::ComPtr<ID3D12Resource> bufferUploader = nullptr;
};
}