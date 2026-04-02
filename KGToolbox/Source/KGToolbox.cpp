
#include "KGToolbox.h"

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <chrono>
#include <iostream>
#include <string>
#include <sstream>

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
	SDL_SetMainReady();
	SDL_SetAppMetadata("KGToolbox", "1.0", "com.kgx.kgtoolbox");

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
		return -1;
	}

	kgt::KGToolboxApp KGTApp(1024, 768);

	return KGTApp.run();
}

namespace kgt
{
KGToolboxApp::KGToolboxApp(int initialWindowWidth, int initialWindowHeight)
	: mSDLWindow(nullptr), mClientWidth(initialWindowWidth), mClientHeight(initialWindowHeight)
{
	mKgxEngine = std::make_unique<KrienGraphiXEngine>();

	//TODO(KL): add some error handling/logging here
	SDL_Init(SDL_INIT_VIDEO);

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
		//updateWindowTitle(deltaTime);

		const float NewRoll = std::fmodf(mBoxObject->getTransform().getRoll() + (deltaTime / 10.0f), glm::two_pi<float>());
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

void KGToolboxApp::updateWindowTitle(float deltaTime) const
{
	static int frameCount = 0;
	static float timeElapsed = 0.0;

	timeElapsed += deltaTime;
	++frameCount;

	if (timeElapsed >= 1.0f)
	{
		const auto fps = static_cast<float>(frameCount);
		const float mspf = 1000.0f / fps;

		const std::string windowTitle = "KGXToolbox";

		std::stringstream ss;
		ss << windowTitle.c_str() << "    fps: " << std::to_string(static_cast<int>(fps));
		ss << "   mspf: " << std::to_string(std::lroundf(mspf));

		SDL_SetWindowTitle(mSDLWindow, ss.str().c_str());

		frameCount = 0;
		timeElapsed -= 1.0f;
	}
}
}
