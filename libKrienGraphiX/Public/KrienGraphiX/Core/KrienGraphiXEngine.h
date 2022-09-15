
#pragma once

#include <memory>
#include <Windows.h>

namespace kgx::core
{
class RenderWindow;
}

class KrienGraphiXEngine final
{
public:
    KrienGraphiXEngine();
    ~KrienGraphiXEngine();

    std::shared_ptr<kgx::core::RenderWindow> createRenderWindow(HWND windowHandle, unsigned int initialWindowWidth, unsigned int initialWindowHeight) const;

private:
    std::shared_ptr<kgx::core::RenderWindow> mRenderWindow;
};
