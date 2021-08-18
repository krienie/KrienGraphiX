
#include "Private/RHI/D3D12/DX12GraphicsDevice.h"
#include "Private/RHI/D3D12/d3dx12Residency.h"

#include "Private/RHI/D3D12/DX12BaseResource.h"

namespace kgx::RHI
{
DX12BaseResource::DX12BaseResource(DX12GraphicsDevice * device, ID3D12Resource * resource)
    : mDevice(device), mDxResource(resource), mResidencyHandle()
{
}

DX12BaseResource::~DX12BaseResource()
{
    if (mResidencyHandle.IsInitialized())
    {
        
    }
}

void DX12BaseResource::startResidencyTracking()
{
    const D3D12_RESOURCE_DESC ResourceDesc = mDxResource->GetDesc();
    const D3D12_RESOURCE_ALLOCATION_INFO Info = mDevice->getNativeDevice()->GetResourceAllocationInfo(0, 1, &ResourceDesc);
    
    //D3DX12Residency::Initialize(ResidencyHandle, Resource.GetReference(), Info.SizeInBytes);
    //D3DX12Residency::BeginTrackingObject(GetParentDevice()->GetResidencyManager(), ResidencyHandle);
}

//void DX12BaseResource::addToResidencySet(DX12GraphicsCommandList * commandList)
//{
//    
//}
} // namespace kgx::RHI