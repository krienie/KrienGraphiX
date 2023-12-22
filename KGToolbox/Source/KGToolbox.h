
#pragma once

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

    int run();

    LRESULT msgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    HWND initWindow() const;
    void updateWindowTitle(float deltaTime) const;

    HINSTANCE mHInstance = nullptr;
    HWND mWindowHandle = nullptr;
    unsigned int mClientWidth;
	unsigned int mClientHeight;

    KrienGraphiXEngine mKgxEngine;
};
}
