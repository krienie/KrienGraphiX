
#pragma once

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl/client.h>

#include "Private/RHI/RHIConstantBuffer.h"

namespace kgx::RHI
{
class RHIGraphicsDevice;

class DX12ConstantBuffer final : public RHIConstantBuffer
{
    public:
        //TODO(KL): Add support for place resource constant buffer
        DX12ConstantBuffer(unsigned int sizeInBytes, CD3DX12_CPU_DESCRIPTOR_HANDLE descHeapHandle);
        ~DX12ConstantBuffer() override = default;

        bool init(RHIGraphicsDevice *device) override;

        void * map(MapType type) override;
        void unmap() override;

    private:
        MapType mCurMapType;
        Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
        CD3DX12_CPU_DESCRIPTOR_HANDLE mDescHeapHandle;
};
}
