
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
        enum class MapType
        {
            READ_ONLY,
            WRITE_ONLY,
            READ_WRITE
        };

        RHIConstantBuffer(size_t sizeInBytes) : mBufferSize(sizeInBytes) {}
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
        virtual void * map(MapType type) = 0;
        virtual void unmap() = 0;

    private:
        std::string mBufferName;
        size_t mBufferSize = 0u;
        size_t mRegister = 0u;
};
}