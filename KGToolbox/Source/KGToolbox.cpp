
#include "framework.h"
#include <string>
#include <sstream>

#include "KGToolbox.h"
#include "Resource.h"

#include "KrienGraphiX/Scene/KGXMeshComponent.h"

namespace
{
kgt::KGToolboxApp* KGToolboxPtr = nullptr;

std::wstring loadResourceWString(HINSTANCE hInstance, int resourceId)
{
    constexpr int maxLoadStringSize = 100;
    std::wstring resourceString;
    resourceString.resize(maxLoadStringSize);

    LoadStringW(hInstance, resourceId, resourceString.data(), maxLoadStringSize);

    return resourceString;
};
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    kgt::KGToolboxApp KGTApp(hInstance, 1024, 768);

    KGToolboxPtr = &KGTApp;

    return KGTApp.run();
}

namespace kgt
{
KGToolboxApp::KGToolboxApp(HINSTANCE hInstance, unsigned int initialWindowWidth, unsigned int initialWindowHeight)
    : mHInstance(hInstance), mClientWidth(initialWindowWidth), mClientHeight(initialWindowHeight)
{
    // Init main window
    mWindowHandle = initWindow();

    // Startup KGX
    mKgxEngine.createRenderWindow(reinterpret_cast<kgx::WinHandle>(mWindowHandle), mClientWidth, mClientHeight);
    mKgxEngine.addSceneUpdateDelegate([this](float deltaTime)
    {
        updateWindowTitle(deltaTime);
    });

    //BoxObject = mKgxEngine.spawnNewObject<kgx::KGXSceneObject>("BoxObject");
    BoxObject = std::make_unique<kgx::KGXSceneObject>("BoxObject");
    BoxObject->addNewComponent<kgx::KGXMeshComponent>();
}

int KGToolboxApp::run()
{
    MSG msg = {nullptr};
 
	while (msg.message != WM_QUIT)
	{
		if(PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ))
		{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
    }

	return static_cast<int>(msg.wParam);
}

HWND KGToolboxApp::initWindow() const
{
    const std::wstring windowTitle = loadResourceWString(mHInstance, IDS_APP_TITLE);
    const std::wstring windowClass = loadResourceWString(mHInstance, IDC_KGTOOLBOX);

    WNDCLASSEXW wndClassEx;
    wndClassEx.cbSize = sizeof(WNDCLASSEX);
    wndClassEx.style          = CS_HREDRAW | CS_VREDRAW;
    wndClassEx.lpfnWndProc    = MainWndProc;
    wndClassEx.cbClsExtra     = 0;
    wndClassEx.cbWndExtra     = 0;
    wndClassEx.hInstance      = mHInstance;
    wndClassEx.hIcon          = LoadIcon(mHInstance, MAKEINTRESOURCE(IDI_KGTOOLBOX));
    wndClassEx.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wndClassEx.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wndClassEx.lpszMenuName   = MAKEINTRESOURCEW(IDC_KGTOOLBOX);
    wndClassEx.lpszClassName  = windowClass.c_str();
    wndClassEx.hIconSm        = LoadIcon(wndClassEx.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    
	if( !RegisterClassExW(&wndClassEx) )
	{
		MessageBox(0, L"RegisterClass Failed.", nullptr, 0);
		return nullptr;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT clientRect = { 0, 0, static_cast<long>(mClientWidth), static_cast<long>(mClientHeight) };
    AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, false);
	const int width  = clientRect.right - clientRect.left;
	const int height = clientRect.bottom - clientRect.top;
    
	const auto windowHandle = CreateWindow(wndClassEx.lpszClassName, windowTitle.c_str(), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, mHInstance, nullptr); 
	if( !windowHandle )
	{
		MessageBox(0, L"CreateWindow Failed.", nullptr, 0);
		return nullptr;
	}

	ShowWindow(windowHandle, SW_SHOW);
	UpdateWindow(windowHandle);

	return windowHandle;
}

void KGToolboxApp::updateWindowTitle(float deltaTime) const
{
    static int frameCount = 0;
    static float timeElapsed = 0.0;

    timeElapsed += deltaTime;
    ++frameCount;

    if (timeElapsed >= 1.0f)
    {
        const auto fps = static_cast<float>(frameCount);
        const float mspf = 1000.0f / fps;

        const std::wstring windowTitle = loadResourceWString(mHInstance, IDS_APP_TITLE);

        std::wstringstream wss;
        wss << windowTitle.c_str() << L"    fps: " << std::to_wstring(static_cast<int>(fps));
        wss << L"   mspf: " << std::to_wstring(std::lroundf(mspf));

        SetWindowText(mWindowHandle, wss.str().c_str());

        frameCount = 0;
        timeElapsed -= 1.0f;
    }
}

LRESULT KGToolboxApp::msgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int messageId = LOWORD(wParam);
            // Parse the menu selections:
            switch (messageId)
            {
            case IDM_ABOUT:
                DialogBox(mHInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return KGToolboxPtr->msgProc(hWnd, message, wParam, lParam);
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;

    default:
        break;
    }
    return (INT_PTR)FALSE;
}
