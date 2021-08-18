
#pragma once

#include "Private/RHI/RHIDepthStencilBuffer.h"

#include <memory>

namespace kgx::Rendering
{
class RcDepthStencilBuffer final
{
    public:
        RcDepthStencilBuffer(unsigned int width, unsigned int height);
        ~RcDepthStencilBuffer() = default;

    private:
        std::unique_ptr<RHI::RHIDepthStencilBuffer> mRHIDepthStencilBuffer;
};
}
