
#pragma once

#include "DX12Resource.h"
#include "Private/RHI/RHIBuffer.h"

namespace kgx::RHI
{
class DX12GraphicsDevice;

class DX12UploadBuffer final : public RHIBuffer, public DX12Resource
{
    public:
        DX12UploadBuffer(DX12GraphicsDevice* dxDevice, const RHIBufferDescriptor& descriptor);
        ~DX12UploadBuffer() override = default;

        DX12UploadBuffer(DX12UploadBuffer&) = delete;
        DX12UploadBuffer(DX12UploadBuffer&&) noexcept = default;
        DX12UploadBuffer& operator=(DX12UploadBuffer&) = delete;
        DX12UploadBuffer& operator=(DX12UploadBuffer&&) noexcept = default;
        
    private:
        void* mapImpl(MapType type) override;
        void unmapImpl() override;
        
        RHIBufferDescriptor mDescriptor;
};
}
