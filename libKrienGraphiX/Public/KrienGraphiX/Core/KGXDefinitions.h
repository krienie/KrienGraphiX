
#pragma once

#include <functional>

namespace kgx
{
using WinHandle = unsigned int*;
using SceneUpdateDelegate = std::function<void(float deltaTime)>;
}
