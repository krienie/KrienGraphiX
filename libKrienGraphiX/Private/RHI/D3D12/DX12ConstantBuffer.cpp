
#include "DX12ConstantBuffer.h"
#include "DX12Descriptors.h"
#include "DX12GraphicsDevice.h"
#include "DX12MemoryUtils.h"

namespace kgx::RHI
{
DX12ConstantBuffer::DX12ConstantBuffer(size_t sizeInBytes, DX12ConstantBufferDescriptor descriptor)
    : RHIConstantBuffer(DX12MemoryUtils::alignTo256Bytes(sizeInBytes)), mResource(nullptr),
    mDescriptor(std::move(descriptor))
{
}

bool DX12ConstantBuffer::init(RHIGraphicsDevice* device)
{
    auto* dxDevice = dynamic_cast<DX12GraphicsDevice*>(device);
    if (dxDevice == nullptr)
    {
        // This should never happen
        return false;
    }

    auto* nativeDevice = dxDevice->getNativeDevice();

    const D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize());
    const HRESULT res = nativeDevice->CreatePlacedResource(mDescriptor.heap.Get(), mDescriptor.heapOffset, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mResource));

    if (FAILED(res))
    {
        return false;
    }

    // Describe and create a constant buffer view.
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = mResource->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = static_cast<UINT>(bufferSize());
    nativeDevice->CreateConstantBufferView(&cbvDesc, mDescriptor.descriptorHandle);

    return true;
}
    
void* DX12ConstantBuffer::mapImpl(MapType type)
{
    void* dataPtr;

    // Write only
    if ((type | MapType::WRITE) == MapType::WRITE)
    {
        const CD3DX12_RANGE readRange(0, 0);
        mResource->Map(0, &readRange, &dataPtr);
    } else
    {
        // TODO(KL): Maybe allow a subset of the buffer to be set for reading. Like, mapSubResource or something
        mResource->Map(0, nullptr, &dataPtr);
    }

    return dataPtr;
}

void DX12ConstantBuffer::unmapImpl()
{
    if ((currentMappedType() | MapType::READ) == MapType::READ)
    {
        const CD3DX12_RANGE writeRange(0, 0);
        mResource->Unmap(0, &writeRange);
    } else
    {
        mResource->Unmap(0, nullptr);
    }
}
}