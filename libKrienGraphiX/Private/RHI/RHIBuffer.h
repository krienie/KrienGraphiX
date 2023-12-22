
#pragma once

#include <string>

#include "RHIDescriptors.h"
#include "RHIViewableResource.h"

namespace kgx::RHI
{
class RHIBuffer : public RHIViewableResource
{
public:
    enum MapType : uint8_t
    {
        READ = 0x1 << 0,
        WRITE = 0x1 << 1,
        READ_WRITE = READ | WRITE
    };

    RHIBuffer(const RHIBufferDescriptor& descriptor);
    ~RHIBuffer() override;

    [[nodiscard]]
    std::string name() const { return mBufferName; }

    [[nodiscard]]
    size_t bufferSize() const { return mBufferSize; }

    [[nodiscard]]
    bool IsBuffer() const override { return true; }

    [[nodiscard]]
    size_t bufferRegister() const { return mRegister; }

    [[nodiscard]]
    void* mappedDataPtr() const { return mMappedDataPtr; }
    
    void* map(MapType type);
    void unmap();

    void copyBufferData(const void* data, unsigned int sizeInBytes, bool keepMapped = false);

protected:
    [[nodiscard]]
    MapType currentMappedType() const { return static_cast<MapType>(mCurrentMappedType); }

private:
    std::string mBufferName;
    size_t mBufferSize = 0u;

    [[nodiscard]]
    virtual void* mapImpl(MapType type) = 0;
    virtual void unmapImpl() = 0;
    
    size_t mRegister = 0u;

    void* mMappedDataPtr = nullptr;
    uint8_t mCurrentMappedType = MapType::READ_WRITE;
};
}
