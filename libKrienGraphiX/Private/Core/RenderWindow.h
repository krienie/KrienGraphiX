
#pragma once

#include <memory>

#include <SDL3/SDL.h>

namespace kgx::rendering
{
class KGXRenderWindow;
}

namespace kgx::core
{
class RenderWindow final
{
public:
	RenderWindow(SDL_Window* window, unsigned int width, unsigned int height);
	~RenderWindow() = default;

	[[nodiscard]] SDL_Window* getWinHandle() const;

	void draw();
	
	//TODO(KL): implement resize

private:
	std::shared_ptr<rendering::KGXRenderWindow> mKGXRenderWindow;
	std::atomic_bool mIsWindowBeingRendered = false;
};
}
