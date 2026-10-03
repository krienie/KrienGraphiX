
#pragma once

#include "KGXSceneObject.h"

#include <memory>
#include <string>

namespace kgx::core
{
namespace KGXAssetLoader
{
std::shared_ptr<KGXSceneObject> loadFromFile(const std::string& filePathString);
};
}


