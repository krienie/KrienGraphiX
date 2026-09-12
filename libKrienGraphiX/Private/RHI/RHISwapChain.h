
#pragma once

#include <SDL3/SDL_video.h>

#include "RHIPixelFormat.h"
#include "RHITextureHandle.h"

namespace kgx::RHI
{
class RHISwapChain
{
	public:
		virtual ~RHISwapChain() = default;

		virtual bool create(SDL_Window* window, unsigned int frameCount, RHIPixelFormat pixelFormat) = 0;

		[[nodiscard]] virtual RHITextureHandle getCurrentBufferView() = 0;
		
		virtual void present() = 0;
};
}
