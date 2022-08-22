
#pragma once

namespace kgx
{
typedef unsigned int * WinHandle;

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
