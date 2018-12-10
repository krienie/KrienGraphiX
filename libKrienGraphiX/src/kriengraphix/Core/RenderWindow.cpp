
#include "Core/RenderWindow.h"

#include <iostream>

#include "Simulation/Camera.h"

namespace kgx
{
	RenderWindow::RenderWindow( ID3D11Device *dxDevice,  IDXGIFactory2 *dxgiFactory )
		: m_dxDev(dxDevice), m_dxDevCont(nullptr), m_dxgiFactory(dxgiFactory), m_swapChain(nullptr),
			m_renderTargetView(nullptr), m_depthStencilView(nullptr), m_rasterizer(nullptr),
			m_curViewport(), m_backBuffWidth(0U), m_backBuffHeight(0U), m_isInit(false)
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

		if ( m_depthStencilView )
			m_depthStencilView->Release();

		if ( m_rasterizer )
			m_rasterizer->Release();

		if ( m_renderTargetView )
			m_renderTargetView->Release();

		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}

	bool RenderWindow::create( HWND windowHandle )
	{
		// create swapchain descriptor
		DXGI_SWAP_CHAIN_DESC1 swapDesc;
		ZeroMemory( &swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC1) );
		swapDesc.Width  = 0;
		swapDesc.Height = 0;
		swapDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
		swapDesc.Stereo = FALSE;

		// If multi-sample antialiasing is being used, all bound render targets and depth buffers must have the same sample counts and quality levels.
		swapDesc.SampleDesc.Count   = 1;
		swapDesc.SampleDesc.Quality = 0; //D3D11_STANDARD_MULTISAMPLE_PATTERN;
		swapDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.BufferCount        = 1;
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
		
		// create and initialize depth-stencil buffer
		ID3D11Texture2D *depthBuffTex = nullptr;
		D3D11_TEXTURE2D_DESC depthBuffDesc;
		depthBuffDesc.Width              = m_backBuffWidth;
		depthBuffDesc.Height             = m_backBuffHeight;
		depthBuffDesc.MipLevels          = 1;
		depthBuffDesc.ArraySize          = 1;
		depthBuffDesc.Format             = DXGI_FORMAT_D32_FLOAT; //DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBuffDesc.Usage              = D3D11_USAGE_DEFAULT;
		depthBuffDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
		depthBuffDesc.CPUAccessFlags     = 0;
		depthBuffDesc.SampleDesc.Count   = backBufferDesc.SampleDesc.Count;
		depthBuffDesc.SampleDesc.Quality = backBufferDesc.SampleDesc.Quality;
		depthBuffDesc.MiscFlags          = 0;

		m_dxDev->CreateTexture2D( &depthBuffDesc, nullptr, &depthBuffTex );
		res = m_dxDev->CreateDepthStencilView( depthBuffTex, nullptr, &m_depthStencilView );
		depthBuffTex->Release();
		if ( FAILED(res) )
		{
			backBuff->Release();
			m_swapChain->Release();
			m_swapChain = nullptr;
			std::cout << "Error (RenderWindow::create): Error creating depthStencilView." << std::endl;
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


		// Setup the rasterizer
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode				 = D3D11_CULL_BACK;		//D3D11_CULL_FRONT, D3D11_CULL_BACK or D3D11_CULL_NONE
		rasterDesc.DepthBias			 = 0;
		rasterDesc.DepthBiasClamp		 = 0.0f;
		rasterDesc.DepthClipEnable		 = true;
		rasterDesc.FillMode				 = D3D11_FILL_SOLID;	//D3D11_FILL_WIREFRAME or D3D11_FILL_SOLID
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable	 = true;
		rasterDesc.ScissorEnable		 = false;
		rasterDesc.SlopeScaledDepthBias  = 0.0f;

		res = m_dxDev->CreateRasterizerState(&rasterDesc, &m_rasterizer);
		if ( FAILED(res) )
		{
			m_renderTargetView->Release();
			m_renderTargetView = nullptr;
			m_swapChain->Release();
			m_swapChain = nullptr;
			std::cout << "Error (RenderWindow::create): Error creating Rasterizer state." << std::endl;
			return false;
		}

		m_isInit = true;
		return true;
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

	unsigned int RenderWindow::getWidth() const
	{
		return m_backBuffWidth;
	}

	unsigned int RenderWindow::getHeight() const
	{
		return m_backBuffHeight;
	}


	void RenderWindow::update()
	{
		if ( !m_isInit || !m_curViewport.cam )
			return;

		//NOTE: We do not need to clear the back-buffer here, because we use a deferred rendering pipeline.
		//      Clearing the gbuffer is done by the GBufferPass.

		// clear the depth-stencil buffer
		m_dxDevCont->ClearDepthStencilView( m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

		m_curViewport.cam->renderCurrentView(m_curViewport.dxViewport, m_rasterizer, m_renderTargetView, m_depthStencilView);

		// flip the back buffer and the front buffer
		m_swapChain->Present(0, 0);
	}
}
