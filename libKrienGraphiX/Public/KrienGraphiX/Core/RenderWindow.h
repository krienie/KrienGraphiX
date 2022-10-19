
#pragma once

#include <memory>

#include "KGXDefinitions.h"

namespace kgx::RHI
{
class RHIDepthStencilBuffer;
class RHISwapChain;
}

namespace kgx::core
{
class RenderWindow final
{
public:
    RenderWindow(WinHandle windowHandle, unsigned int width, unsigned int height);
    ~RenderWindow() = default;

    [[nodiscard]] WinHandle getWinHandle() const { return mWindowHandle; }

    void draw();

    //TODO(KL): implement resize

private:
    WinHandle mWindowHandle;

    std::unique_ptr<RHI::RHISwapChain> mRHISwapChain;

    //TODO(KL): Temporarily here. Move to KGXRenderer
    std::unique_ptr<RHI::RHIDepthStencilBuffer> mDepthStencil;

    KGXViewport mViewport;
};
}
