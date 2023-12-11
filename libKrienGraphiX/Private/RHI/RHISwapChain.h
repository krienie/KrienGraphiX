
#pragma once

#include "RHITexture2D.h"

#include "KrienGraphiX/Core/KGXDefinitions.h"
#include "Private/RHI/RHICommandQueue.h"
#include "Private/RHI/RHIGraphicsDevice.h"

namespace kgx::RHI
{
class RHISwapChain
{
    public:
        virtual ~RHISwapChain() = default;

        virtual bool create(RHIGraphicsDevice* device, RHICommandQueue* commandQueue, WinHandle windowHandle, unsigned int frameCount, RHIPixelFormat pixelFormat) = 0;

        [[nodiscard]] virtual RHIResourceView* getCurrentBufferView() const = 0;
        
        virtual void present() = 0;
};
}
