
#include "DX12Buffer.h"
#include "DX12Descriptors.h"
#include "DX12GraphicsCommandList.h"
#include "DX12GraphicsDevice.h"
#include "DX12MemoryUtils.h"

namespace
{
kgx::RHI::RHIBufferDescriptor getAlignedBufferDescriptor(const kgx::RHI::RHIBufferDescriptor& descriptor)
{
    kgx::RHI::RHIBufferDescriptor alignedDesc = descriptor;
    if (descriptor.isBufferAligned)
    {
        alignedDesc.bufferSize = static_cast<unsigned>(kgx::RHI::DX12MemoryUtils::alignTo256Bytes(alignedDesc.bufferSize));
    }

    return alignedDesc;
}
}

namespace kgx::RHI
{
DX12Buffer::DX12Buffer(DX12GraphicsDevice* dxDevice, DX12GraphicsCommandList* commandList, const RHIBufferDescriptor& descriptor)
    : RHIBuffer(getAlignedBufferDescriptor(descriptor)),
        DX12Resource(nullptr, nullptr, D3D12_RESOURCE_STATE_GENERIC_READ),
        mDescriptor(descriptor)
{
    ID3D12Device* nativeDevice = dxDevice->getNativeDevice();

    D3D12_HEAP_TYPE heapType = mDescriptor.isDynamic ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;
    D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_GENERIC_READ;

    //TODO(KL): refactor... this is a bit messy

    //D3D12_RESOURCE_STATE_COMMON
    if (mDescriptor.initialData)
    {
        initialResourceState = D3D12_RESOURCE_STATE_COPY_DEST;
    }

    size_t calculatedBufferSize = bufferSize();

    if (!mDescriptor.isDynamic)
    {
        // Create upload buffer to help with the buffer update
        const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
        const D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(calculatedBufferSize);
        HRESULT res = nativeDevice->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&bufferUploader));

        if (FAILED(res))
        {
            return;
        }
    }

    // Create the default buffer resource
    const CD3DX12_HEAP_PROPERTIES heapProperties(heapType);
    const D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(calculatedBufferSize);
    HRESULT res = nativeDevice->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            initialResourceState,
            nullptr,
            IID_PPV_ARGS(&mResource));
    
    if (FAILED(res))
    {
        return;
    }

    if (mDescriptor.isDynamic)
    {
        map(MapType::READ_WRITE);
    }

    if (mDescriptor.initialData)
    {
        if (mDescriptor.isDynamic)
        {
            auto mappedData = mappedDataPtr();
            std::memcpy(mappedData, mDescriptor.initialData, calculatedBufferSize);
        }
        else
        {
            D3D12_SUBRESOURCE_DATA subResourceData = {};
            subResourceData.pData = mDescriptor.initialData;
            subResourceData.RowPitch = static_cast<LONG_PTR>(calculatedBufferSize);
            subResourceData.SlicePitch = subResourceData.RowPitch;

            UpdateSubresources<1>(commandList->getCommandList(), mResource.Get(), bufferUploader.Get(), 0, 0, 1, &subResourceData);
            transitionToState(commandList, D3D12_RESOURCE_STATE_GENERIC_READ);
        }
    }
}

void* DX12Buffer::mapImpl(MapType type)
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

void DX12Buffer::unmapImpl()
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