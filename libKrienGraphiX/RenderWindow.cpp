
#include <iostream>

#include "Camera.h"
#include "RenderWindow.h"

namespace kgx
{
	RenderWindow::RenderWindow( ID3D11Device *dxDevice,  IDXGIFactory2 *dxgiFactory )
		: m_dxDev(dxDevice), m_dxDevCont(nullptr), m_dxgiFactory(dxgiFactory), m_swapChain(nullptr),
			m_renderTargetView(nullptr), m_renderCore(dxDevice), m_curViewport(), m_backBuffWidth(0U),
			m_backBuffHeight(0U), m_isInit(false)
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );

		setClearColor( 0.4f, 0.4f, 0.4f );
	}

	RenderWindow::~RenderWindow()
	{
		if ( m_swapChain )
		{
			m_swapChain->SetFullscreenState( FALSE, nullptr );
			m_swapChain->Release();
		}

		if ( m_renderTargetView )
			m_renderTargetView->Release();

		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}

	/*void RenderWindow::enumAdaptersTest()
	{
		IDXGIAdapter1 *adapter = nullptr;
		HRESULT res = m_dxgiFactory->EnumAdapters1( 0u, &adapter );

		DXGI_ADAPTER_DESC1 adapterDesc;
		ZeroMemory( &adapterDesc, sizeof(DXGI_ADAPTER_DESC1) );
		adapter->GetDesc1( &adapterDesc );

		UINT i = 0;
		IDXGIOutput *output;
		adapter->EnumOutputs( 0u, &output );
		DXGI_OUTPUT_DESC outputDesc;
		ZeroMemory( &outputDesc, sizeof(DXGI_OUTPUT_DESC) );
		output->GetDesc( &outputDesc );

		UINT numModes = 0u;
		output->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, 0u, &numModes, nullptr );
		DXGI_MODE_DESC *modes = new DXGI_MODE_DESC[numModes];
		output->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, 0u, &numModes, modes );

		DXGI_MODE_DESC lastMode = modes[numModes - 1u];

		std::cout << "giggety" << std::endl;

		delete[] modes;
		output->Release();
		adapter->Release();
	}*/

	bool RenderWindow::create( HWND windowHandle )
	{
		// create swapchain descriptor
		DXGI_SWAP_CHAIN_DESC1 swapDesc;
		ZeroMemory( &swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC1) );
		swapDesc.Width  = 0;
		swapDesc.Height = 0;
		swapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.Stereo = FALSE;

		// If multi-sample antialiasing is being used, all bound render targets and depth buffers must have the same sample counts and quality levels.
		swapDesc.SampleDesc.Count   = 4;
		swapDesc.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
		swapDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.BufferCount        = 2;
		swapDesc.Scaling            = DXGI_SCALING_STRETCH;
		swapDesc.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;
		swapDesc.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapDesc.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// create fullscreen swapchain descriptor
		/*DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenSwapDesc;
		ZeroMemory( &fullscreenSwapDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC) );
		fullscreenSwapDesc.RefreshRate      = { 60000, 1000 };
		fullscreenSwapDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
		fullscreenSwapDesc.Windowed         = TRUE;
		fullscreenSwapDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;*/

		//enumAdaptersTest();

		HRESULT res = m_dxgiFactory->CreateSwapChainForHwnd( m_dxDev, windowHandle, &swapDesc, nullptr, nullptr, &m_swapChain );

		if ( FAILED(res) )
		{
			std::cout << "Error (RenderWindow::create): Error creating swapchain." << std::endl;
			return false;
		}


		// create and initialize backbuffer
		ID3D11Texture2D *backBuff;
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuff));
		D3D11_TEXTURE2D_DESC backBufferDesc;
		backBuff->GetDesc(&backBufferDesc);

		m_backBuffWidth  = backBufferDesc.Width;
		m_backBuffHeight = backBufferDesc.Height;
		
		// setup RenderCore => contains depthbuffer and rasterizer
		bool renderCoreInit = m_renderCore.init( m_backBuffWidth, m_backBuffHeight, swapDesc.SampleDesc.Count, swapDesc.SampleDesc.Quality );
		if ( !renderCoreInit )
		{
			backBuff->Release();
			m_swapChain->Release();
			m_swapChain = nullptr;
			std::cout << "Error (RenderWindow::create): Error initializing RenderCore." << std::endl;
			return false;
		}

		// create renderTarget from the backBuffer and depth stencil view
		res = m_dxDev->CreateRenderTargetView( backBuff, nullptr, &m_renderTargetView );
		backBuff->Release();

		if ( FAILED(res) )
		{
			m_swapChain->Release();
			m_swapChain = nullptr;
			std::cout << "Error (RenderWindow::create): Error creating RenderTargetView." << std::endl;
			return false;
		}

		m_isInit = true;
		return true;
	}


	RenderCore* RenderWindow::getRenderCorePtr()
	{
		return &m_renderCore;
	}


	void RenderWindow::setViewport( Camera *cam, float topLeftX, float topLeftY, float width, float height )
	{
		if ( !m_isInit )
		{
			std::cout << "Error (RenderWindow::setViewport): RenderWindow is not initialized. Call RenderWindow::create() first." << std::endl;
			abort();
		}

		// initialize viewport
		D3D11_VIEWPORT dxViewport;
		ZeroMemory(&dxViewport, sizeof(D3D11_VIEWPORT));

		if ( topLeftX + width <= D3D11_VIEWPORT_BOUNDS_MAX )
		{
			dxViewport.TopLeftX = topLeftX;

			if ( width < 0.0f )
				dxViewport.Width = static_cast<float>(m_backBuffWidth);
			else dxViewport.Width = width;
			
		} else
		{
			std::cout << "Warning (RenderWindow::addViewport): Viewport out of bounds. Clamping viewport." << std::endl;
			dxViewport.TopLeftX = 0.0f;
			dxViewport.Width    = D3D11_VIEWPORT_BOUNDS_MAX;
		}

		if ( topLeftY + height <= D3D11_VIEWPORT_BOUNDS_MAX )
		{
			dxViewport.TopLeftY = topLeftY;

			if ( height < 0.0f )
				dxViewport.Height = static_cast<float>(m_backBuffHeight);
			else dxViewport.Height = height;
			
		} else
		{
			std::cout << "Warning (RenderWindow::addViewport): Viewport out of bounds. Clamping viewport." << std::endl;
			dxViewport.TopLeftY = 0.0f;
			dxViewport.Height   = D3D11_VIEWPORT_BOUNDS_MAX;
		}

		dxViewport.MinDepth = D3D11_MIN_DEPTH;
		dxViewport.MaxDepth = D3D11_MAX_DEPTH;

		m_curViewport = Viewport(dxViewport, cam);
	}

	void RenderWindow::setClearColor( float red, float green, float blue )
	{
		m_clearColor[0] = red;
		m_clearColor[1] = green;
		m_clearColor[2] = blue;
		m_clearColor[3] = 1.0f;
	}

	bool RenderWindow::isFullscreen() const
	{
		BOOL fullscreen = FALSE;
		if ( m_swapChain )
			m_swapChain->GetFullscreenState( &fullscreen, nullptr );

		return fullscreen != FALSE;
	}
	void RenderWindow::setFullscreen( bool active )
	{
		if ( m_swapChain )
			m_swapChain->SetFullscreenState( active, nullptr );
	}


	float RenderWindow::getAspectRatio() const
	{
		return m_backBuffWidth / static_cast<float>(m_backBuffHeight);
	}


	void RenderWindow::update()
	{
		if ( !m_isInit || !m_curViewport.cam )
			return;

		//TODO: add support for multiple viewports
		m_dxDevCont->RSSetViewports( 1, &m_curViewport.dxViewport );
		m_dxDevCont->OMSetRenderTargets( 1, &m_renderTargetView, m_renderCore.getDepthStencilView() );

		// clear the back buffer
		m_dxDevCont->ClearRenderTargetView( m_renderTargetView, m_clearColor );

		m_renderCore.renderFrame( m_curViewport.cam );

		// flip the back buffer and the front buffer
		m_swapChain->Present(1, 0);
	}
}
