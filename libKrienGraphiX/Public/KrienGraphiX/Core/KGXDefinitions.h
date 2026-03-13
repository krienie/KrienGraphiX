
#pragma once

#include <functional>

namespace kgx
{
using SceneUpdateDelegate = std::function<void(float deltaTime)>;
}
