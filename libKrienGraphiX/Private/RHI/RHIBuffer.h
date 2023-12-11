
#pragma once

#include <string>

#include "RHIDescriptors.h"
#include "RHIViewableResource.h"

namespace kgx::RHI
{
class RHIBuffer : public RHIViewableResource
{
public:
    RHIBuffer(const RHIBufferDescriptor& descriptor)
        : RHIViewableResource(descriptor.flags), mBufferName(descriptor.name), mBufferSize(descriptor.bufferSize) {}
    ~RHIBuffer() override = default;

    [[nodiscard]]
    std::string name() const { return mBufferName; }

    [[nodiscard]]
    size_t bufferSize() const { return mBufferSize; }

    [[nodiscard]]
    bool IsBuffer() const override { return true; }

private:
    std::string mBufferName;
    size_t mBufferSize = 0u;
};
}
