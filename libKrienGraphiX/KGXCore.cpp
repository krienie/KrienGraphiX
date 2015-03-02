
#define _CRT_SECURE_NO_DEPRECATE		//to disable deprecation warnings about freopen()

#include <iostream>

#include "ResourceManager.h"
#include "KGXCore.h"

namespace kgx
{
	KGXCore* KGXCore::inst = 0;

	KGXCore* KGXCore::getInst()
	{
		if ( !KGXCore::inst )
			inst = new KGXCore();

		return inst;
	}

	void KGXCore::destroy()
	{
		if ( KGXCore::inst )
			delete KGXCore::inst;
	}

	KGXCore::KGXCore()
		: dxDev(nullptr), dxDevCont(nullptr), dxgiFactory(nullptr), renderWindows()
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
											D3D11_SDK_VERSION, &dxDev, NULL, &dxDevCont );

		if ( FAILED(res) )
		{
			std::cout << "Error (KGXCore::KGXCore): Error creating D3D11 device." << std::endl;
			abort();							//TODO: throw proper exception explaining what the problem is
		}

		// retrieve default DXGIAdaptor
		IDXGIDevice2 *dxgiDevice = NULL;
		res = dxDev->QueryInterface(__uuidof(IDXGIDevice2), (void **)&dxgiDevice);
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

		res = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void **)&dxgiFactory);
		if ( FAILED(res) )
		{
			std::cout << "Error (KGXCore::KGXCore): Error retrieving DXGIFactory2." << std::endl;
			abort();
		}
		dxgiAdapter->Release();


		// init ResourceManager
		ResourceManager::construct(dxDev);
	}

	KGXCore::~KGXCore()
	{
		//renderWindows.clear();
		std::map<HWND, RenderWindow*>::iterator it;
		for ( it = renderWindows.begin(); it != renderWindows.end(); ++it )
			delete it->second;

		ResourceManager::destroy();

		if ( dxgiFactory )
			dxgiFactory->Release();
		if ( dxDevCont )
			dxDevCont->Release();

		//ID3D11Debug *d3dDebug = nullptr;
		//dxDev->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));

		if ( dxDev )
			dxDev->Release();

		//d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		//d3dDebug->Release();
	}


	/** Utility function that returns a direct pointer to the DirectX device */
	ID3D11Device* KGXCore::getDxDevice() const
	{
		return dxDev;
	}


	RenderWindow* KGXCore::createRenderWindow( HWND windowHandle )
	{
		std::map<HWND, RenderWindow*>::iterator it = renderWindows.find(windowHandle);
		if ( it != renderWindows.end() )
		{
			std::cout << "Aborted (Environment::createRenderWindow): Window handle " << windowHandle << " already has a RenderWindow bound." << std::endl;
			//TODO: insert assert(false)
			return NULL;
		}

		RenderWindow *renWin = new RenderWindow( dxDev, dxgiFactory );
		renWin->create(windowHandle);

		renderWindows.insert( std::pair<HWND, RenderWindow*>(windowHandle, renWin) );

		return renWin;
	}


	void KGXCore::renderFrame()
	{
		std::map<HWND, RenderWindow*>::iterator it;
		for ( it = renderWindows.begin(); it != renderWindows.end(); ++it )
			it->second->update();
	}
}
