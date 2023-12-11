
#pragma once

#include <memory>

#include "KrienGraphiX/Core/KGXDefinitions.h"

#include "Private/Core/PrivateDefinitions.h"
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

    std::shared_ptr<RHI::RHISwapChain> mRHISwapChain;
    std::shared_ptr<RHI::RHIResourceView> mSwapChainRTV;

    //TODO(KL): Temporarily here. Move to KGXRenderer
    std::shared_ptr<RHI::RHITexture2D> mDepthStencil;
    std::shared_ptr<RHI::RHIResourceView> mDSV;

    core::KGXViewport mViewport;
};
}
