
#pragma once

#include <SDL3/SDL_video.h>

#include "RHITexture2D.h"
#include "Private/RHI/RHICommandQueue.h"

namespace kgx::RHI
{
class RHISwapChain
{
	public:
		virtual ~RHISwapChain() = default;

		virtual bool create(RHICommandQueue* commandQueue, SDL_Window* window, unsigned int frameCount, RHIPixelFormat pixelFormat) = 0;

		[[nodiscard]] virtual RHIResourceView* getCurrentBufferView() = 0;
		
		virtual void present() = 0;
};
}
