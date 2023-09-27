
#pragma once

#include <functional>

namespace kgx
{
typedef unsigned int* WinHandle;

using SceneUpdateDelegate = std::function<void(float deltaTime)>;
}
