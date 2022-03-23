
#pragma once

#include "Private/RHI/RHISwapChain.h"

#include <memory>

namespace kgx::core
{
typedef RHI::WinHandle WinHandle;

class RenderWindow final
{
    public:
        RenderWindow(WinHandle windowHandle, unsigned int width, unsigned int height);
        ~RenderWindow() = default;

    private:
        std::unique_ptr<RHI::RHISwapChain> mRHISwapChain;
};
}
