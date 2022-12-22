
#pragma once

#include <memory>

#include "KGXDefinitions.h"

namespace kgx::rendering
{
class KGXRenderWindow;
}

namespace kgx::core
{
class RenderWindow final
{
public:
    RenderWindow(WinHandle windowHandle, unsigned int width, unsigned int height);
    ~RenderWindow() = default;

    [[nodiscard]] WinHandle getWinHandle() const;

    void draw();
    
    //TODO(KL): implement resize

private:
    std::shared_ptr<rendering::KGXRenderWindow> mKGXRenderWindow;
};
}
