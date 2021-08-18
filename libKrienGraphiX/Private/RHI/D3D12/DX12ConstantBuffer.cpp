
#include "DX12ConstantBuffer.h"

#include <cassert>


#include "DX12GraphicsDevice.h"

namespace
{
unsigned int alignTo256Bytes(unsigned int inSize)
{
    const unsigned int alignedSize = ((inSize - 1u) / 256u + 1u) * 256u;

    // Constant buffers must be 256-byte aligned
    assert(alignedSize % 256 == 0);

    return alignedSize;
}
}

namespace kgx::RHI
{
DX12ConstantBuffer::DX12ConstantBuffer(unsigned int sizeInBytes, CD3DX12_CPU_DESCRIPTOR_HANDLE descHeapHandle)
    : RHIConstantBuffer(alignTo256Bytes(sizeInBytes)), mCurMapType(MapType::READ_WRITE), mResource(nullptr), mDescHeapHandle(descHeapHandle)
{
}

bool DX12ConstantBuffer::init(RHIGraphicsDevice* device)
{
    auto * dxDevice = dynamic_cast<DX12GraphicsDevice*>(device);
    if (dxDevice == nullptr)
    {
        // This should never happen
        return false;
    }

    auto * nativeDevice = dxDevice->getNativeDevice();

    // Create committed resource for the constant buffer data to live in
    const HRESULT res = nativeDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(bufferSize()),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mResource));

    if (FAILED(res))
    {
        return false;
    }

    // Describe and create a constant buffer view.
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = mResource->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = bufferSize();
    nativeDevice->CreateConstantBufferView(&cbvDesc, mDescHeapHandle);

    return true;
}

void* DX12ConstantBuffer::map(MapType type)
{
    void * dataPtr;
    mCurMapType = type;

    if (type == MapType::WRITE_ONLY)
    {
        CD3DX12_RANGE readRange(0, 0);
        mResource->Map(0, &readRange, &dataPtr);
    } else
    {
        // TODO(KL): Maybe allow a subset of the buffer to be set for reading. Like, mapSubResource or something
        mResource->Map(0, nullptr, &dataPtr);
    }

    return dataPtr;
}

void DX12ConstantBuffer::unmap()
{
    if (mCurMapType == MapType::READ_ONLY)
    {
        CD3DX12_RANGE writeRange(0, 0);
        mResource->Unmap(0, &writeRange);
    } else
    {
        mResource->Unmap(0, nullptr);
    }
}

void DX12ConstantBuffer::copyBufferData(const void* data, unsigned int sizeInBytes)
{
    assert(sizeInBytes <= bufferSize());

    void* dataBegin = map(MapType::WRITE_ONLY);
    memcpy(dataBegin, data, sizeInBytes);
    unmap();
}
} // namespace kgx::RHI