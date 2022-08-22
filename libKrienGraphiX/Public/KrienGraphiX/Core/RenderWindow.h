
#pragma once

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

    void draw();

    //TODO(KL): implement resize

private:
    std::unique_ptr<RHI::RHISwapChain> mRHISwapChain;
    KGXViewport mViewport;
};
}
