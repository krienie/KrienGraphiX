
#include "KGToolbox.h"

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <chrono>
#include <iostream>
#include <string>

#include "KrienGraphiX/Core/Logging.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
	SDL_SetMainReady();
	SDL_SetAppMetadata("KGToolbox", "1.0", "com.kgx.kgtoolbox");

	kgt::KGToolboxApp KGTApp(1024, 768);

	return KGTApp.run();
}

namespace kgt
{
KGToolboxApp::KGToolboxApp(int initialWindowWidth, int initialWindowHeight)
	: mSDLWindow(nullptr), mClientWidth(initialWindowWidth), mClientHeight(initialWindowHeight)
{
	mKgxEngine = std::make_unique<KrienGraphiXEngine>();

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		KGXLOG_CRITICAL("SDL_Init failed: {}", SDL_GetError());
		return;
	}

	SDL_WindowFlags windowFlags = SDL_WINDOW_RESIZABLE;
#ifdef __APPLE__
	windowFlags |= SDL_WINDOW_METAL;
#endif

	const std::string windowTitle = "KGToolboxApp";
	mSDLWindow = SDL_CreateWindow(windowTitle.c_str(), mClientWidth, mClientHeight, SDL_WINDOW_RESIZABLE);

	mKgxEngine->createRenderWindow(mSDLWindow, mClientWidth, mClientHeight);

	mCameraObject = std::make_unique<kgx::KGXCameraObject>("CameraObject");
	mBoxObject = std::make_unique<kgx::KGXBoxObject>("BoxObject");
	
	mKgxEngine->setSceneUpdateDelegate([this]([[maybe_unused]] float deltaTime)
	{
		const float NewRoll = std::fmodf(mBoxObject->getTransform().getRoll() + (deltaTime / 40.0f), glm::two_pi<float>());
		mBoxObject->setRotation(0, 0, NewRoll);
	});
}

int KGToolboxApp::run()
{
	bool running = true;
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
			{
				running = false;
			}
			else if (event.type == SDL_EVENT_KEY_DOWN)
			{
				if (event.key.scancode == SDL_SCANCODE_ESCAPE)
				{
					running = false;
				}
			}
		}
	}

	mKgxEngine.reset();

	if (mSDLWindow)
	{
		SDL_DestroyWindow(mSDLWindow);
	}
	SDL_Quit();

	return 0;
}
}
