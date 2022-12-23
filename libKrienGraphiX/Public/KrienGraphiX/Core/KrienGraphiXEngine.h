
#pragma once

#include "KGXDefinitions.h"

class KrienGraphiXEngine final
{
public:
    KrienGraphiXEngine();
    ~KrienGraphiXEngine();

    bool createRenderWindow(kgx::WinHandle windowHandle, unsigned int initialWindowWidth, unsigned int initialWindowHeight) const;
    void addSceneUpdateDelegate(kgx::SceneUpdateDelegate updateDelegate) const;
};
