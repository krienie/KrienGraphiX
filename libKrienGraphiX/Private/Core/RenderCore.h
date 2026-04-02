
#pragma once

#include "CommandThread.h"
#include "RenderThread.h"
#include "FrameTimer.h"
#include "Private/Scene/KGXScene.h"

#include <SDL3/SDL.h>

#include <memory>
#include <unordered_map>

namespace kgx::core
{
class RenderWindow;

class RenderCore final
{
public:
	static RenderCore* get();
	static void startup();
	static void shutdown();

	RenderCore(const RenderCore&) noexcept            = delete;
	RenderCore& operator=(const RenderCore&) noexcept = delete;
	RenderCore(RenderCore&&) noexcept                 = delete;
	RenderCore& operator=(RenderCore&&) noexcept      = delete;

	[[nodiscard]] KGXScene* getScenePtr() const;

	bool createRenderWindow(SDL_Window* window, unsigned int initialWindowWidth, unsigned int initialWindowHeight);

private:
	RenderCore();
	~RenderCore();

	static RenderCore* mInst;
	static int mRefCount;

	std::unique_ptr<FrameTimer> mFrameTimer;

	std::unique_ptr<KGXScene> mScene;

	std::mutex mRenderWindowMutex;
	std::unordered_map<SDL_Window*, std::shared_ptr<RenderWindow>> mRenderWindows;
};
}
