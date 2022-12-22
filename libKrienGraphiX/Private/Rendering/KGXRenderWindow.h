
#pragma once

#include <memory>

#include "KrienGraphiX/Core/KGXDefinitions.h"

#include "Private/RHI/RHIDepthStencilBuffer.h"
#include "Private/RHI/RHISwapChain.h"

namespace kgx::rendering
{
class KGXRenderWindow final
{
public:
    KGXRenderWindow(WinHandle windowHandle, unsigned int width, unsigned int height);
    ~KGXRenderWindow() = default;

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
