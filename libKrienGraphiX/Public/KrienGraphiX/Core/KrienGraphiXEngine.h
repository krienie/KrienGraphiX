
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
    void addSceneUpdateDelegate(kgx::SceneUpdateDelegate updateDelegate) const;

    //template<class ObjType,
    //            std::enable_if_t<std::is_base_of_v<kgx::KGXSceneObject, ObjType>, int> = 0,
    //            typename... Args>
    //std::weak_ptr<ObjType> spawnNewObject(const std::string& name, Args... args)
    //{
    //    ObjType *newObj = new ObjType(name, args...);
    //    return spawnNewObjectInternal(newObj);
    //}

//private:
//    std::weak_ptr<kgx::KGXSceneObject> spawnNewObjectInternal(kgx::KGXSceneObject* newObject);
};
