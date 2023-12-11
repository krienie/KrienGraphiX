

#include "RHIConstantBuffer.h"

#include <cassert>

namespace kgx::RHI
{
RHIConstantBuffer::RHIConstantBuffer(const RHIConstantBufferDescriptor& cbDesc)
    : RHIBuffer(cbDesc), mRegister(cbDesc.bufferRegister)
{
}

RHIConstantBuffer::~RHIConstantBuffer()
{
    unmap();
}

void* RHIConstantBuffer::map(MapType type)
{
    unmap();

    mMappedDataPtr = mapImpl(type);

    mCurrentMappedType = type;
    return mMappedDataPtr;
}

void RHIConstantBuffer::unmap()
{
    if (mMappedDataPtr)
    {
        unmapImpl();
        mCurrentMappedType = 0u;
        mMappedDataPtr = nullptr;
    }
}

void RHIConstantBuffer::copyBufferData(const void* data, unsigned int sizeInBytes, bool keepMapped)
{
    assert(sizeInBytes <= bufferSize());

    if ((mCurrentMappedType & MapType::WRITE) != MapType::WRITE)
    {
        map(MapType::WRITE);
    }
    
    memcpy(mMappedDataPtr, data, sizeInBytes);

    if (!keepMapped)
    {
        unmap();
    }
}
}
