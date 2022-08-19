
#pragma once

#include <memory>
#include <string>
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

    kgx::core::RenderWindow* createRenderWindow( HWND windowHandle );
    //RenderWindow* getRenderWindow() const;

private:
    //std::shared_ptr<kgx::core::RenderWindow> 
};
