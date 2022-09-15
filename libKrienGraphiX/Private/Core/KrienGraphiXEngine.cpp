
#include "KrienGraphiX/Core/KrienGraphiXEngine.h"

#include "KrienGraphiX/Core/RenderWindow.h"
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

std::shared_ptr<RenderWindow> KrienGraphiXEngine::createRenderWindow(HWND windowHandle, unsigned int initialWindowWidth, unsigned int initialWindowHeight) const
{
    return std::make_shared<RenderWindow>(reinterpret_cast<kgx::WinHandle>(windowHandle), initialWindowWidth, initialWindowHeight);
}
