
#pragma once

#include "RHITexture2D.h"

#include "KrienGraphiX/Core/KGXDefinitions.h"
#include "Private/RHI/RHICommandQueue.h"

namespace kgx::RHI
{
class RHISwapChain
{
	public:
		virtual ~RHISwapChain() = default;

		virtual bool create(RHICommandQueue* commandQueue, WinHandle windowHandle, unsigned int frameCount, RHIPixelFormat pixelFormat) = 0;

		[[nodiscard]] virtual RHIResourceView* getCurrentBufferView() const = 0;
		
		virtual void present() = 0;
};
}
