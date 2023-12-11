
#pragma once

#include <unordered_map>
#include <string>

#include "RHIBuffer.h"
#include "RHIDescriptors.h"

namespace kgx::RHI
{
class RHIGraphicsDevice;

class RHIConstantBuffer : public RHIBuffer
{
public:
    enum MapType : uint8_t
    {
        READ = 0x1 << 0,
        WRITE = 0x1 << 1,
        READ_WRITE = READ | WRITE
    };

    RHIConstantBuffer(const RHIConstantBufferDescriptor& cbDesc);
    ~RHIConstantBuffer() override;
    RHIConstantBuffer(RHIConstantBuffer&) = delete;
    RHIConstantBuffer(RHIConstantBuffer&&) noexcept = default;
    RHIConstantBuffer& operator=(RHIConstantBuffer&) = delete;
    RHIConstantBuffer& operator=(RHIConstantBuffer&&) noexcept = default;

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
    [[nodiscard]]
    virtual void* mapImpl(MapType type) = 0;
    virtual void unmapImpl() = 0;
    
    size_t mRegister = 0u;

    void* mMappedDataPtr = nullptr;
    uint8_t mCurrentMappedType = MapType::READ_WRITE;
};
}
