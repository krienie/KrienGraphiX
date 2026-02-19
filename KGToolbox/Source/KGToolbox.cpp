
#include "KGToolbox.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <sstream>

int SDL_main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
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
	//TODO(KL): add some error handling/logging here
	SDL_Init(SDL_INIT_VIDEO);

	const std::string windowTitle = "KGToolboxApp";
	mSDLWindow = SDL_CreateWindow(windowTitle.c_str(), mClientWidth, mClientHeight, SDL_WINDOW_RESIZABLE);

	const SDL_PropertiesID props = SDL_GetWindowProperties(mSDLWindow);
	const kgx::WinHandle mWindowHandle = static_cast<kgx::WinHandle>(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));

	mCameraObject = std::make_unique<kgx::KGXCameraObject>("CameraObject");
	mBoxObject = std::make_unique<kgx::KGXBoxObject>("BoxObject");

	mKgxEngine.createRenderWindow(mWindowHandle, mClientWidth, mClientHeight);
	mKgxEngine.addSceneUpdateDelegate([this](float deltaTime)
	{
		updateWindowTitle(deltaTime);
	});
	
	mKgxEngine.addSceneUpdateDelegate([this]([[maybe_unused]] float deltaTime)
	{
		const float NewRoll = std::fmodf(mBoxObject->getTransform().getRoll() + deltaTime, DirectX::XM_2PI);
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
		}
	}

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
