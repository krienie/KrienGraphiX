
#include "RHIBuffer.h"

#include <cassert>

namespace kgx::RHI
{
RHIBuffer::RHIBuffer(const RHIBufferDescriptor& descriptor)
    : RHIViewableResource(descriptor.flags),
    mBufferName(descriptor.name),
    mBufferSize(descriptor.bufferSize),
    mRegister(descriptor.bufferRegister)
{
}

RHIBuffer::~RHIBuffer()
{
    unmap();
}

void* RHIBuffer::map(MapType type)
{
    unmap();

    mMappedDataPtr = mapImpl(type);

    mCurrentMappedType = type;
    return mMappedDataPtr;
}

void RHIBuffer::unmap()
{
    if (mMappedDataPtr)
    {
        unmapImpl();
        mCurrentMappedType = 0u;
        mMappedDataPtr = nullptr;
    }
}

void RHIBuffer::copyBufferData(const void* data, unsigned sizeInBytes, bool keepMapped)
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
