
#include "KGToolbox.h"

#include "KrienGraphiX/Core/Logging.h"
#include "KrienGraphiX/Scene/KGXAssetLoader.h"

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <chrono>
#include <iostream>
#include <string>


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

	mBoxObject2 = std::make_unique<kgx::KGXBoxObject>("BoxObject2");
	mBoxObject2->setPosition(kgx::math::Vector3(4.0, 10, 0));

	//TODO(KL): Provide a standard directory or some other way to register where to find models
	mFBXSceneObject = kgx::core::KGXAssetLoader::loadFromFile("/Users/krien/Projects/Models/jerrycan_ucuwaddfa_low/Jerrycan_ucuwaddfa_Low.fbx");

	mFBXSceneObject->setPosition(kgx::math::Vector3(0, 20, 5));
	mFBXSceneObject->setRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0, 0)));
	mFBXSceneObject->setScale(glm::vec3(0.25f));

	mKgxEngine->setSceneUpdateDelegate([this]([[maybe_unused]] float deltaTime)
	{
		{
			const glm::quat quatRotation = mBoxObject->getTransform().getRotation();
			constexpr float rotationSpeed = glm::radians(45.0f);
			glm::quat newRotation = glm::rotate(quatRotation, rotationSpeed * deltaTime, glm::vec3(0.0f, 0.0f, 1.0f));
			mBoxObject->setRotation(glm::normalize(newRotation));
		}

		{
			const glm::quat quatRotation = mBoxObject2->getTransform().getRotation();
			constexpr float rotationSpeed = glm::radians(22.5f);
			glm::quat newRotation = glm::rotate(quatRotation, rotationSpeed * deltaTime, glm::vec3(0.0f, 1.0f, 1.0f));
			mBoxObject2->setRotation(glm::normalize(newRotation));
		}

		if (mFBXSceneObject)
		{
			const glm::quat quatRotation = mFBXSceneObject->getTransform().getRotation();
			constexpr float rotationSpeed = glm::radians(22.5f);
			glm::quat newRotation = glm::rotate(quatRotation, rotationSpeed * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
			mFBXSceneObject->setRotation(glm::normalize(newRotation));
		}
	});
}

int KGToolboxApp::run()
{
	constexpr float cameraMovementSpeed = 1.0f;

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
				else if (event.key.scancode == SDL_SCANCODE_W)
				{
					mCameraObject->getCamera()->moveForward(cameraMovementSpeed);
				}
				else if (event.key.scancode == SDL_SCANCODE_S)
				{
					mCameraObject->getCamera()->moveBackward(cameraMovementSpeed);
				}
				else if (event.key.scancode == SDL_SCANCODE_A)
				{
					mCameraObject->getCamera()->moveLeft(cameraMovementSpeed);
				}
				else if (event.key.scancode == SDL_SCANCODE_D)
				{
					mCameraObject->getCamera()->moveRight(cameraMovementSpeed);
				}
			}
		}
	}

	shutdown();

	if (mSDLWindow)
	{
		SDL_DestroyWindow(mSDLWindow);
	}
	SDL_Quit();

	return 0;
}

void KGToolboxApp::shutdown()
{
	mBoxObject.reset();
	mBoxObject2.reset();
	mFBXSceneObject.reset();
	mKgxEngine.reset();
}
}
