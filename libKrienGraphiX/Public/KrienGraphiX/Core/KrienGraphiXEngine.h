
#pragma once

#include "KGXDefinitions.h"

class KrienGraphiXEngine final
{
public:
	KrienGraphiXEngine();
	~KrienGraphiXEngine();

	bool createRenderWindow(struct SDL_Window* window, unsigned int initialWindowWidth, unsigned int initialWindowHeight) const;
	void setSceneUpdateDelegate(kgx::SceneUpdateDelegate updateDelegate) const;
};
