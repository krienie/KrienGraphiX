
#pragma once

//#include "Private/RHI/RHISwapChain.h"

#include <memory>

#include "KGXDefinitions.h"

namespace kgx::RHI
{
class RHISwapChain;
}

namespace kgx::core
{
class RenderWindow final
{
public:
    RenderWindow(WinHandle windowHandle, unsigned int width, unsigned int height);
    ~RenderWindow() = default;

private:
    std::unique_ptr<RHI::RHISwapChain> mRHISwapChain;
};
}
