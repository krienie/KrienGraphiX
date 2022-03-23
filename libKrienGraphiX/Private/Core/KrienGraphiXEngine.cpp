
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

//TODO(KL): Temporary maybe_unused
RenderWindow* KrienGraphiXEngine::createRenderWindow([[maybe_unused]] HWND windowHandle)
{
    // Create window
    return nullptr;
}
