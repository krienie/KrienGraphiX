
#pragma once

#include "RHITexture2D.h"

#include <memory>

namespace kgx::RHI
{
class RHICommandQueue;
class RHIGraphicsDevice;

typedef unsigned int * WinHandle;

class RHISwapChain
{
    public:
        virtual ~RHISwapChain() = default;

        virtual bool init(RHIGraphicsDevice *device, RHICommandQueue * commandQueue, WinHandle windowHandle, unsigned int frameCount) = 0;

        [[nodiscard]]
        virtual unsigned int getFrameIndex() const = 0;

        [[nodiscard]]
        virtual std::weak_ptr<RHITexture2D> getBuffer(unsigned int idx) const = 0;
};
}