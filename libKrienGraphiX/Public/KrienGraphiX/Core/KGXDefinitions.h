
#pragma once

#include <functional>

namespace kgx
{
typedef unsigned int* WinHandle;

using SceneUpdateDelegate = std::function<void(float deltaTime)>;

struct KGXViewport
{
    int topLeftX;
    int topLeftY;
    unsigned int width;
    unsigned int height;
    float minDepth;
    float maxDepth;
};
}
