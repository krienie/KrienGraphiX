
#include "KrienGraphiX/Core/KrienGraphiXEngine.h"

#include "Private/Core/RenderCore.h"

using namespace kgx::core;


KrienGraphiXEngine::KrienGraphiXEngine()
{
    RenderCore::startup();
}

KrienGraphiXEngine::~KrienGraphiXEngine()
{
    RenderCore::shutdown();
}

bool KrienGraphiXEngine::createRenderWindow(kgx::WinHandle windowHandle, unsigned int initialWindowWidth, unsigned int initialWindowHeight) const
{
    return RenderCore::get()->createRenderWindow(windowHandle, initialWindowWidth, initialWindowHeight);
}

void KrienGraphiXEngine::addSceneUpdateDelegate(kgx::SceneUpdateDelegate updateDelegate) const
{
    RenderCore::get()->getSceneThreadPtr()->addSceneUpdateDelegate(std::move(updateDelegate));
}
