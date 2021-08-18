
#pragma once

#include <unordered_map>
#include <string>
#include <vector>

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

        RHIConstantBuffer(unsigned int sizeInBytes) : mBufferSize(sizeInBytes) {}
        virtual ~RHIConstantBuffer() = default;
        RHIConstantBuffer(RHIConstantBuffer &) = delete;
        RHIConstantBuffer(RHIConstantBuffer &&) noexcept = delete;
        RHIConstantBuffer & operator=(RHIConstantBuffer &) = delete;
        RHIConstantBuffer & operator=(RHIConstantBuffer &&) noexcept = delete;

        virtual bool init(RHIGraphicsDevice *device) = 0;

        [[nodiscard]]
        std::string name() const { return mBufferName; }

        [[nodiscard]]
        unsigned int bufferSize() const { return mBufferSize; }

        [[nodiscard]]
        virtual void * map(MapType type) = 0;
        virtual void unmap() = 0;

    private:
        std::string mBufferName;
        unsigned int mBufferSize = 0u;
        unsigned int mRegister = 0u;
};
}