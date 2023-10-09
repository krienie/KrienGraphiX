
#pragma once

#include "RHITexture2D.h"

#include "KrienGraphiX/Core/KGXDefinitions.h"
#include "Private/RHI/RHICommandQueue.h"
#include "Private/RHI/RHIGraphicsDevice.h"

#include <memory>

namespace kgx::RHI
{
class RHISwapChain
{
    public:
        virtual ~RHISwapChain() = default;

        virtual bool create(RHIGraphicsDevice* device, RHICommandQueue* commandQueue, WinHandle windowHandle, unsigned int frameCount, RHIPixelFormat pixelFormat) = 0;

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
