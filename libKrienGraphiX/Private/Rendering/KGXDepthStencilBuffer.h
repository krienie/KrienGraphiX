
#pragma once

#include "Private/RHI/RHIDepthStencilBuffer.h"

#include <memory>

namespace kgx::Rendering
{
class KGXDepthStencilBuffer final
{
    public:
        KGXDepthStencilBuffer(unsigned int width, unsigned int height);
        ~KGXDepthStencilBuffer() = default;

    private:
        std::unique_ptr<RHI::RHIDepthStencilBuffer> mRHIDepthStencilBuffer;
};
}
