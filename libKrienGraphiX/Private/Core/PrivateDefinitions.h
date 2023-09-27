
#pragma once

namespace kgx::core
{
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
