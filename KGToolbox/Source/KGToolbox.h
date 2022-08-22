
#pragma once

#include "resource.h"

#include <KrienGraphiX/Core/KrienGraphiXEngine.h>

namespace kgt
{
class KGToolboxApp final
{
public:
    KGToolboxApp(HINSTANCE hInstance, unsigned int initialWindowWidth, unsigned int initialWindowHeight);
    KGToolboxApp(const KGToolboxApp& rhs) = delete;
    KGToolboxApp& operator=(const KGToolboxApp& rhs) = delete;
    ~KGToolboxApp() = default;

    int Run();

    LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    HWND InitWin32Window() const;

    HINSTANCE mHInstance = nullptr;
    unsigned int mClientWidth;
	unsigned int mClientHeight;

    KrienGraphiXEngine mKgxEngine;
    std::shared_ptr<kgx::core::RenderWindow> mRenderWindow;
};
}
