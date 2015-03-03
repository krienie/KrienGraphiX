
#define _CRT_SECURE_NO_DEPRECATE		//to disable deprecation warnings about freopen()

#include <DirectXMath.h>
#include <iostream>

#include "RenderWindow.h"
#include "ResourceManager.h"
#include "KGXCore.h"

namespace kgx
{
	KGXCore* KGXCore::m_inst = 0;

	KGXCore* KGXCore::getInst()
	{
		if ( !KGXCore::m_inst )
			m_inst = new KGXCore();

		return m_inst;
	}

	void KGXCore::destroy()
	{
		if ( KGXCore::m_inst )
			delete KGXCore::m_inst;
	}

	KGXCore::KGXCore()
		: m_dxDev(nullptr), m_dxDevCont(nullptr), m_dxgiFactory(nullptr), m_renderWindows()
	{
		// open console when compiling for debugging
#ifdef _DEBUG
		AllocConsole();
		AttachConsole( GetCurrentProcessId() );

#pragma warning(suppress: 6031)
		freopen( "CON", "w", stdout );					//redirect stdout to the created console
#endif

		// make sure SSE/SSE2 instructions are supported on current hardware
		if ( !DirectX::XMVerifyCPUSupport() )
		{
			std::cout << "Error (KGXCore::KGXCore): SSE/SSE2 instructions are not supported on current hardware." << std::endl;
			abort();							//TODO: throw proper exception explaining what the problem is
		}


		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
		// If the project is in a debug build, enable debugging via SDK Layers with this flag.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// create ordering of featureLevels
		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0,
											  /*D3D_FEATURE_LEVEL_10_1,
											  D3D_FEATURE_LEVEL_10_0,
											  D3D_FEATURE_LEVEL_9_3,
											  D3D_FEATURE_LEVEL_9_1*/ };

		HRESULT res = D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
											creationFlags, featureLevels, ARRAYSIZE(featureLevels),
											D3D11_SDK_VERSION, &m_dxDev, NULL, &m_dxDevCont );

		if ( FAILED(res) )
		{
			std::cout << "Error (KGXCore::KGXCore): Error creating D3D11 device." << std::endl;
			abort();							//TODO: throw proper exception explaining what the problem is
		}

		// retrieve default DXGIAdaptor
		IDXGIDevice2 *dxgiDevice = NULL;
		res = m_dxDev->QueryInterface(__uuidof(IDXGIDevice2), (void **)&dxgiDevice);
		if ( FAILED(res) )
		{
			std::cout << "Error (KGXCore::KGXCore): Error retrieving DXGIDevice2." << std::endl;
			abort();
		}

		IDXGIAdapter2 *dxgiAdapter = NULL;
		res = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&dxgiAdapter);
		if ( FAILED(res) )
		{
			std::cout << "Error (KGXCore::KGXCore): Error retrieving DXGIAdapter2." << std::endl;
			abort();
		}
		dxgiDevice->Release();

		res = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void **)&m_dxgiFactory);
		if ( FAILED(res) )
		{
			std::cout << "Error (KGXCore::KGXCore): Error retrieving DXGIFactory2." << std::endl;
			abort();
		}
		dxgiAdapter->Release();


		// init ResourceManager
		ResourceManager::construct(m_dxDev);
	}

	KGXCore::~KGXCore()
	{
		//m_renderWindows.clear();
		std::map<HWND, RenderWindow*>::iterator it;
		for ( it = m_renderWindows.begin(); it != m_renderWindows.end(); ++it )
			delete it->second;

		ResourceManager::destroy();

		if ( m_dxgiFactory )
			m_dxgiFactory->Release();
		if ( m_dxDevCont )
			m_dxDevCont->Release();

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
		std::map<HWND, RenderWindow*>::iterator it = m_renderWindows.find(windowHandle);
		if ( it != m_renderWindows.end() )
		{
			std::cout << "Aborted (Environment::createRenderWindow): Window handle " << windowHandle << " already has a RenderWindow bound." << std::endl;
			//TODO: insert assert(false)
			return NULL;
		}

		RenderWindow *renWin = new RenderWindow( m_dxDev, m_dxgiFactory );
		renWin->create(windowHandle);

		m_renderWindows.insert( std::pair<HWND, RenderWindow*>(windowHandle, renWin) );

		return renWin;
	}


	void KGXCore::renderFrame()
	{
		std::map<HWND, RenderWindow*>::iterator it;
		for ( it = m_renderWindows.begin(); it != m_renderWindows.end(); ++it )
			it->second->update();
	}
}
