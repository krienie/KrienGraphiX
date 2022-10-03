
#pragma once

#include "RHITexture2D.h"

#include <memory>

#include "KrienGraphiX/Core/KGXDefinitions.h"

namespace kgx::RHI
{
class RHICommandQueue;
class RHIGraphicsDevice;

class RHISwapChain
{
    public:
        virtual ~RHISwapChain() = default;

        virtual bool init(RHIGraphicsDevice *device, RHICommandQueue * commandQueue, WinHandle windowHandle, unsigned int frameCount, RHIPixelFormat pixelFormat) = 0;

        [[nodiscard]] virtual RHITexture2D* getCurrentBuffer() const = 0;
        [[nodiscard]] virtual RHIResourceView* getCurrentBufferRTV() const
        {
            if (RHITexture2D* currentBuffer = getCurrentBuffer())
            {
                return currentBuffer->getResourceViewByType(RHIResourceView::RTV).get();
            }

            return nullptr;
        }
        
        virtual void present() = 0;
};
}
