
#pragma once

#include <unordered_map>
#include <string>

namespace kgx::RHI
{
class RHIGraphicsDevice;

struct RHIBufferVariable
{
    std::string name;
    unsigned int offset;
    unsigned int size;
};

class RHIConstantBuffer
{
public:
    enum MapType : uint8_t
    {
        READ = 0x1 << 0,
        WRITE = 0x1 << 1,
        READ_WRITE = READ | WRITE
    };

    RHIConstantBuffer(size_t sizeInBytes);
    virtual ~RHIConstantBuffer() = default;
    RHIConstantBuffer(RHIConstantBuffer &) = delete;
    RHIConstantBuffer(RHIConstantBuffer &&) noexcept = default;
    RHIConstantBuffer & operator=(RHIConstantBuffer &) = delete;
    RHIConstantBuffer & operator=(RHIConstantBuffer &&) noexcept = default;

    virtual bool init(RHIGraphicsDevice *device) = 0;

    [[nodiscard]]
    std::string name() const { return mBufferName; }

    [[nodiscard]]
    size_t bufferSize() const { return mBufferSize; }

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
    
    std::string mBufferName;
    size_t mBufferSize = 0u;
    size_t mRegister = 0u;

    void* mMappedDataPtr = nullptr;
    uint8_t mCurrentMappedType = MapType::READ_WRITE;
};
}