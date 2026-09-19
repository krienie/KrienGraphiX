
#pragma once

#include <SDL3/SDL_video.h>

#include <KrienGraphiX/Core/KrienGraphiXEngine.h>
#include "KrienGraphiX/Scene/KGXSceneObject.h"

namespace kgt
{
class KGToolboxApp final
{
public:
	KGToolboxApp(int initialWindowWidth, int initialWindowHeight);
	~KGToolboxApp() = default;

	KGToolboxApp(const KGToolboxApp& rhs) = delete;
	KGToolboxApp& operator=(const KGToolboxApp& rhs) = delete;

	int run();

private:
	void shutdown();

	SDL_Window* mSDLWindow;
	int mClientWidth;
	int mClientHeight;

	std::unique_ptr<KrienGraphiXEngine> mKgxEngine;
	std::unique_ptr<kgx::KGXBoxObject> mBoxObject;
	std::unique_ptr<kgx::KGXBoxObject> mBoxObject2;
	std::unique_ptr<kgx::KGXCameraObject> mCameraObject;
};
}
