
#include "DX12UploadBuffer.h"
#include "DX12Descriptors.h"
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
DX12UploadBuffer::DX12UploadBuffer(DX12GraphicsDevice* dxDevice, const RHIBufferDescriptor& descriptor)
    : RHIBuffer(getAlignedBufferDescriptor(descriptor)),
        DX12Resource(nullptr, nullptr, D3D12_RESOURCE_STATE_GENERIC_READ)
    , mDescriptor(descriptor)
{
    ID3D12Device* nativeDevice = dxDevice->getNativeDevice();

    const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
    const D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize());
    HRESULT res = nativeDevice->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mResource));
    
    //TODO(KL): Add below optimisation later to allow the creation of one big heap where all ConstantBuffers of the frame live in
    //const HRESULT res = nativeDevice->CreatePlacedResource(mDescriptor.heap.Get(), mDescriptor.heapOffset, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mResource));

    if (FAILED(res))
    {
        return;
    }

    map(MapType::READ_WRITE);

    //constexpr bool IsShaderVisible = true;
    //addResourceView(std::make_shared<DX12ResourceView>(DX12ResourceView::ViewType::CBV, std::shared_ptr<DX12ConstantBuffer>(this), IsShaderVisible));
}

void* DX12UploadBuffer::mapImpl(MapType type)
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

void DX12UploadBuffer::unmapImpl()
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