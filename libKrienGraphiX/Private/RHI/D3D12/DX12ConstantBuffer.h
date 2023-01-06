
#pragma once

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl/client.h>

#include "DX12Descriptors.h"
#include "Private/RHI/RHIConstantBuffer.h"

namespace kgx::RHI
{
class RHIGraphicsDevice;

class DX12ConstantBuffer final : public RHIConstantBuffer
{
    public:
        DX12ConstantBuffer(size_t sizeInBytes, DX12ConstantBufferDescriptor descriptor);
        ~DX12ConstantBuffer() override = default;

        DX12ConstantBuffer(DX12ConstantBuffer &) = delete;
        DX12ConstantBuffer(DX12ConstantBuffer &&) noexcept = default;
        DX12ConstantBuffer & operator=(DX12ConstantBuffer &) = delete;
        DX12ConstantBuffer & operator=(DX12ConstantBuffer &&) noexcept = default;

        bool init(RHIGraphicsDevice *device) override;
        
    private:
        void* mapImpl(MapType type) override;
        void unmapImpl() override;
        
        Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
        DX12ConstantBufferDescriptor mDescriptor;
};
}
