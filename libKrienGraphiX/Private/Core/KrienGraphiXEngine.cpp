
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
    RenderCore::get()->getScenePtr()->addSceneUpdateDelegate(std::move(updateDelegate));
}

//std::weak_ptr<kgx::KGXSceneObject> KrienGraphiXEngine::spawnNewObjectInternal(kgx::KGXSceneObject* newObject)
//{
//    const std::shared_ptr<kgx::KGXSceneObject> NewObjectPtr(newObject);
//    RenderCore::get()->getScenePtr()->addSceneObject(NewObjectPtr);
//
//    return NewObjectPtr;
//}
