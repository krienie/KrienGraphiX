
#pragma once

#include "Private/RHI/RHISwapChain.h"

#include <memory>

namespace kgx::core
{
typedef RHI::WinHandle WinHandle;

class RcRenderWindow final
{
    public:
        RcRenderWindow(WinHandle windowHandle, unsigned int width, unsigned int height);
        ~RcRenderWindow() = default;

    private:
        std::unique_ptr<RHI::RHISwapChain> mRHISwapChain;
};
}
