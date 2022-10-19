
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

    int run();

    LRESULT msgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    HWND initWindow() const;
    void updateWindowTitle(double deltaTime);

    HINSTANCE mHInstance = nullptr;
    unsigned int mClientWidth;
	unsigned int mClientHeight;

    KrienGraphiXEngine mKgxEngine;
    std::shared_ptr<kgx::core::RenderWindow> mRenderWindow;

    // Frame time data
    double mSecondsPerCount;
	double mDeltaTime;
    __int64 mPrevTime;
};
}
