
#define _CRT_SECURE_NO_DEPRECATE		//to disable deprecation warnings about freopen()

#include "Core/KGXCore.h"

#include <chrono>

#include <DirectXMath.h>
#include <iostream>

#include "Core/SceneThread.h"
#include "Core/RenderThread.h"
#include "Core/RenderWindow.h"
#include "Core/ConfigManager.h"
#include "IO/Filesystem.h"
#include "Rendering/TextureLoader.h"

namespace kgx
{
    KGXCore* KGXCore::m_inst = nullptr;

    KGXCore* KGXCore::get()
    {
        if ( !m_inst )
            m_inst = new KGXCore();

        return m_inst;
    }

    void KGXCore::shutdown()
    {
        delete m_inst;
    }

    KGXCore::KGXCore()
        : m_isRunning(false), m_dxDev(nullptr),
            m_dxgiFactory(nullptr), m_mainClockThread(nullptr),
            m_renderWindow(nullptr), m_sceneThread(nullptr), m_renderThread(nullptr)
    {
        // open console when compiling for debugging
//#ifdef _DEBUG
        AllocConsole();
        AttachConsole(GetCurrentProcessId());

#pragma warning(suppress: 6031)
        freopen( "CON", "w", stdout );      //redirect stdout to the created console
//#endif

        // make sure SSE/SSE2 instructions are supported on current hardware
        if ( !DirectX::XMVerifyCPUSupport() )
        {
            std::cout << "Error (KGXCore::KGXCore): SSE/SSE2 instructions are not supported on current hardware." << std::endl;
            abort();
        }


        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
        // If the project is in a debug build, enable debugging via SDK Layers with this flag.
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        // create ordering of featureLevels
        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1 };

        HRESULT res = D3D11CreateDevice( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                         creationFlags, featureLevels, ARRAYSIZE( featureLevels ),
                                         D3D11_SDK_VERSION, &m_dxDev, nullptr, nullptr );

        if ( FAILED( res ) )
        {
            std::cout << "Error (KGXCore::KGXCore): Error creating D3D11 device." << std::endl;
            abort();
        }

        // retrieve default DXGIAdaptor
        IDXGIDevice2 *dxgiDevice = nullptr;
        res = m_dxDev->QueryInterface( __uuidof(IDXGIDevice2), (void**)&dxgiDevice );
        if ( FAILED( res ) )
        {
            std::cout << "Error (KGXCore::KGXCore): Error retrieving DXGIDevice2." << std::endl;
            abort();
        }

        IDXGIAdapter2 *dxgiAdapter = nullptr;
        res = dxgiDevice->GetParent( __uuidof(IDXGIAdapter2), (void**)&dxgiAdapter );
        if ( FAILED( res ) )
        {
            std::cout << "Error (KGXCore::KGXCore): Error retrieving DXGIAdapter2." << std::endl;
            abort();
        }
        dxgiDevice->Release();

        res = dxgiAdapter->GetParent( __uuidof(IDXGIFactory2), (void**)&m_dxgiFactory );
        if ( FAILED( res ) )
        {
            std::cout << "Error (KGXCore::KGXCore): Error retrieving DXGIFactory2." << std::endl;
            abort();
        }
        dxgiAdapter->Release();
    }

    KGXCore::~KGXCore()
    {
        if ( m_mainClockThread )
        {
            m_isRunning = false;
            m_mainClockThread->join();
            delete m_mainClockThread;
        }

        delete m_sceneThread;
        delete m_renderThread;
        delete m_renderWindow;

        // destroy managers
        ConfigManager::destroy();
        TextureLoader::closeThread();

        if ( m_dxgiFactory )
            m_dxgiFactory->Release();

        //ID3D11Debug *d3dDebug = nullptr;
        //m_dxDev->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));

        if ( m_dxDev )
            m_dxDev->Release();

        //d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        //d3dDebug->Release();
    }


    /** Utility function that returns a direct pointer to the DirectX device */
    ID3D11Device* KGXCore::getDxDevicePtr() const
    {
        return m_dxDev;
    }


    RenderWindow* KGXCore::createRenderWindow( HWND windowHandle )
    {
        if ( m_renderWindow )
        {
            std::cout << "Aborted (KGXCore::createRenderWindow): Window handle " << windowHandle << " already has a RenderWindow bound." << std::endl;
            return m_renderWindow;
        }

        m_renderWindow = new RenderWindow( m_dxDev, m_dxgiFactory );
        if ( !m_renderWindow->create( windowHandle ) )
        {
            delete m_renderWindow;
            return nullptr;
        }

        return m_renderWindow;
    }

    RenderWindow* KGXCore::getRenderWindow() const
    {
        return m_renderWindow;
    }

    bool KGXCore::isRunning() const
    {
        return m_isRunning;
    }

    void KGXCore::startMainLoop(RenderWindow *renderWindow)
    {
        if ( !renderWindow )
        {
            std::cout << "Aborted (KGXCore::startMainLoop): A valid RenderWindow has not been created. Please call KGXCore::createRenderWindow() first." << std::endl;
            return;
        }
        
        // Start all threads and start the main clock when ready
        m_sceneThread  = new SceneThread();
        m_renderThread = new RenderThread(m_dxDev);
        m_prevClockTime   = std::chrono::high_resolution_clock::now();
        m_mainClockThread = new std::thread( &KGXCore::clockTick, this );

        getSceneThread()->addSceneView(renderWindow);

        m_isRunning = true;
    }

    //TODO: rename or remove
    void KGXCore::clearManagers()
    {
        filesystem::clearSearchPaths();
    }

    SceneThread * KGXCore::getSceneThread() const
    {
        return m_sceneThread;
    }

    RenderThread *KGXCore::getRenderThread() const
    {
        return m_renderThread;
    }

    void KGXCore::clockTick()
    {
        while (m_isRunning)
        {
            std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> deltaTime = currentTime - m_prevClockTime;

            constexpr std::chrono::duration<double> frameLimit(1.0 / 400.0);

            if (deltaTime > frameLimit)
            {
                //const long long curFrameNum = getRenderThread()->getFrameNumber();
                //std::cout << "KGXCore::clockTick: " << curFrameNum << std::endl;


                // TODO: add an FPS limit

                m_sceneThread->advance( deltaTime.count() );

                m_prevClockTime = currentTime;
            }
        }
    }
}
