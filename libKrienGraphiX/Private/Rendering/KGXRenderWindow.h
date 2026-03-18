
#pragma once

#include <memory>

#include "KrienGraphiX/Core/KGXDefinitions.h"

#include "Private/Core/PrivateDefinitions.h"
#include "Private/RHI/RHISwapChain.h"

namespace kgx::RHI
{
	class RHIGraphicsPipelineState;
}

namespace kgx::rendering
{
class KGXRenderWindow final
{
public:
	KGXRenderWindow(SDL_Window* window, unsigned int width, unsigned int height);
	~KGXRenderWindow() = default;

	[[nodiscard]] SDL_Window* getWinHandle() const { return mWindowHandle; }

	void draw() const;

	//TODO(KL): implement resize

private:
	SDL_Window* mWindowHandle;

	std::shared_ptr<RHI::RHISwapChain> mRHISwapChain;

	//TODO(KL): Temporarily here. Move to KGXRenderer
	std::shared_ptr<RHI::RHITexture2D> mDepthStencil;
	std::shared_ptr<RHI::RHIResourceView> mDSV;

	core::KGXViewport mViewport{};
};
}
