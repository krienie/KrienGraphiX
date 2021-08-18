
#pragma once

#include "Private/RHI/D3D12/DX12GraphicsDevice.h"
#include "Private/RHI/D3D12/d3dx12Residency.h"

namespace kgx::RHI
{
class DX12BaseResource
{
    public:
        DX12BaseResource(DX12GraphicsDevice * device, ID3D12Resource * resource);
        virtual ~DX12BaseResource();

        void startResidencyTracking();
        //void addToResidencySet(DX12GraphicsCommandList * commandList);

    private:
        DX12GraphicsDevice * mDevice;
        ID3D12Resource * mDxResource;
        D3DX12Residency::ManagedObject mResidencyHandle;
};
}