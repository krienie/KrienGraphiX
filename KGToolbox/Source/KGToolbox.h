
#pragma once

#include "resource.h"

#include <KrienGraphiX/Core/KrienGraphiXEngine.h>

namespace kgt
{
class KGToolboxApp final
{
public:
    KGToolboxApp(HINSTANCE hInstance, int initialWindowWidth, int initialWindowHeight);
    KGToolboxApp(const KGToolboxApp& rhs) = delete;
    KGToolboxApp& operator=(const KGToolboxApp& rhs) = delete;
    ~KGToolboxApp() = default;

    int Run();

    LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    HWND InitWin32Window() const;

    HINSTANCE mHInstance = nullptr;
    int mClientWidth;
	int mClientHeight;

    KrienGraphiXEngine mKgxEngine;
};
}
