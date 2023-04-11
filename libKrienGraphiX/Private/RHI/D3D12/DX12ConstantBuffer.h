
#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "DX12Descriptors.h"
#include "DX12Resource.h"
#include "Private/RHI/RHIConstantBuffer.h"

namespace kgx::RHI
{
class DX12GraphicsDevice;

class DX12ConstantBuffer final : public RHIConstantBuffer, public DX12Resource
{
    public:
        DX12ConstantBuffer(DX12GraphicsDevice* dxDevice, const RHIConstantBufferDescriptor& descriptor);
        ~DX12ConstantBuffer() override = default;

        DX12ConstantBuffer(DX12ConstantBuffer &) = delete;
        DX12ConstantBuffer(DX12ConstantBuffer &&) noexcept = default;
        DX12ConstantBuffer & operator=(DX12ConstantBuffer &) = delete;
        DX12ConstantBuffer & operator=(DX12ConstantBuffer &&) noexcept = default;
        
    private:
        void* mapImpl(MapType type) override;
        void unmapImpl() override;
        
        RHIConstantBufferDescriptor mDescriptor;
};
}
