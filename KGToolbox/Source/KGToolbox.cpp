// KGToolbox.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "KGToolbox.h"

namespace
{
kgt::KGToolboxApp* KGToolboxPtr = nullptr;
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

    return KGTApp.Run();
}

namespace kgt
{
KGToolboxApp::KGToolboxApp(HINSTANCE hInstance, int initialWindowWidth, int initialWindowHeight)
    : mHInstance(hInstance), mClientWidth(initialWindowWidth), mClientHeight(initialWindowHeight)
{
    // Init main window
    auto windowHandle = InitWin32Window();

    // Startup KGX
    //TODO(KL): Startup KGX
}

int KGToolboxApp::Run()
{
    MSG msg = {nullptr};
 
	//mTimer.Reset();

	while(msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ))
		{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
        {
            //TODO(KL): call all active RenderWindows
			//mTimer.Tick();
            //
			//if( !mAppPaused )
			//{
			//	CalculateFrameStats();
			//	Update(mTimer);	
            //    Draw(mTimer);
			//}
			//else
			{
				Sleep(100);
			}
        }
    }

	return static_cast<int>(msg.wParam);
}

HWND KGToolboxApp::InitWin32Window() const
{
    auto loadResourceWString = [this](int resourceId)
    {
        constexpr int maxLoadStringSize = 100;
        std::wstring resourceString;
        resourceString.resize(maxLoadStringSize);

        LoadStringW(mHInstance, resourceId, resourceString.data(), maxLoadStringSize);

        return resourceString;
    };

    const std::wstring windowTitle = loadResourceWString(IDS_APP_TITLE);
    const std::wstring windowClass = loadResourceWString(IDC_KGTOOLBOX);

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
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT clientRect = { 0, 0, mClientWidth, mClientHeight };
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

LRESULT KGToolboxApp::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
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
    //case WM_PAINT:
    //    {
    //        PAINTSTRUCT ps;
    //        HDC hdc = BeginPaint(hWnd, &ps);
    //        // TODO: Add any drawing code that uses hdc here...
    //        EndPaint(hWnd, &ps);
    //    }
    //    break;
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
    return KGToolboxPtr->MsgProc(hWnd, message, wParam, lParam);
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
