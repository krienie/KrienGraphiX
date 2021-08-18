
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

void KrienGraphiXEngine::render()
{
    RenderCore::get()->render();
}
