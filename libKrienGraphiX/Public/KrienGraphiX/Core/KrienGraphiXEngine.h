
#pragma once

#include <memory>

#include "KGXDefinitions.h"
#include "KrienGraphiX/Scene/KGXSceneObject.h"

class KrienGraphiXEngine final
{
public:
	KrienGraphiXEngine();
	~KrienGraphiXEngine();

	bool createRenderWindow(kgx::WinHandle windowHandle, unsigned int initialWindowWidth, unsigned int initialWindowHeight) const;
	void setSceneUpdateDelegate(kgx::SceneUpdateDelegate updateDelegate) const;
};
