
#include <iostream>

#include "Camera.h"
#include "RenderWindow.h"

namespace kgx
{
	RenderWindow::RenderWindow( _In_ ID3D11Device *dxDevice, _In_ IDXGIFactory2 *dxgiFactory )
		: m_dxDev(dxDevice), m_dxDevCont(0), m_dxgiFactory(dxgiFactory), m_swapChain(0), m_depthStencilView(0),
			m_renderTargetView(0), m_rasterizer(0), m_curViewport(), m_backBuffWidth(0U), m_backBuffHeight(0U),
			m_isInit(false)
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );

		setClearColor( 0.4f, 0.4f, 0.4f, 1.0f );
	}

	RenderWindow::~RenderWindow()
	{
		if ( m_rasterizer )
			m_rasterizer->Release();

		if ( m_renderTargetView )
			m_renderTargetView->Release();

		if ( m_depthStencilView )
			m_depthStencilView->Release();

		if ( m_swapChain )
			m_swapChain->Release();

		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}


	bool RenderWindow::create( HWND windowHandle )
	{
		// create m_swapChain descriptor
		DXGI_SWAP_CHAIN_DESC1 swapDesc;
		ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
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
		HRESULT res = m_dxgiFactory->CreateSwapChainForHwnd( m_dxDev, windowHandle, &swapDesc, NULL, NULL, &m_swapChain );

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
		ID3D11Texture2D *depthBuff = NULL;
		D3D11_TEXTURE2D_DESC depthBuffDesc;
		depthBuffDesc.Width              = backBufferDesc.Width;
		depthBuffDesc.Height             = backBufferDesc.Height;
		depthBuffDesc.MipLevels          = 1;
		depthBuffDesc.ArraySize          = 1;
		depthBuffDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBuffDesc.Usage              = D3D11_USAGE_DEFAULT;
		depthBuffDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
		depthBuffDesc.CPUAccessFlags     = 0;
		depthBuffDesc.SampleDesc.Count   = swapDesc.SampleDesc.Count;
		depthBuffDesc.SampleDesc.Quality = swapDesc.SampleDesc.Quality;
		depthBuffDesc.MiscFlags          = 0;

		m_dxDev->CreateTexture2D( &depthBuffDesc, NULL, &depthBuff );
		res = m_dxDev->CreateDepthStencilView( depthBuff, NULL, &m_depthStencilView );
		depthBuff->Release();

		if ( FAILED(res) )
		{
			backBuff->Release();
			std::cout << "Error (RenderWindow::create): Error creating DepthStencilView." << std::endl;
			return false;
		}
		

		// create renderTarget from the backBuffer and depth stencil view
		res = m_dxDev->CreateRenderTargetView(backBuff, NULL, &m_renderTargetView);
		backBuff->Release();

		if ( FAILED(res) )
		{
			std::cout << "Error (RenderWindow::create): Error creating RenderTargetView." << std::endl;
			return false;
		}

		// Setup the m_rasterizer
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode				 = D3D11_CULL_BACK;			//D3D11_CULL_FRONT, D3D11_CULL_BACK or D3D11_CULL_NONE
		rasterDesc.DepthBias			 = 0;
		rasterDesc.DepthBiasClamp		 = 0.0f;
		rasterDesc.DepthClipEnable		 = true;
		rasterDesc.FillMode				 = D3D11_FILL_WIREFRAME;	//D3D11_FILL_WIREFRAME or D3D11_FILL_SOLID
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable	 = false;
		rasterDesc.ScissorEnable		 = false;
		rasterDesc.SlopeScaledDepthBias  = 0.0f;

		res = m_dxDev->CreateRasterizerState(&rasterDesc, &m_rasterizer);
		if ( FAILED(res) )
		{
			std::cout << "Error (RenderWindow::create): Error creating Rasterizer state." << std::endl;
			return false;
		}

		m_isInit = true;
		return true;
	}


	void RenderWindow::setViewport( _In_ Camera *cam, float topLeftX, float topLeftY, float width, float height )
	{
		// initialize viewport
		D3D11_VIEWPORT dxViewport;
		ZeroMemory(&dxViewport, sizeof(D3D11_VIEWPORT));

		if ( width > 0 && topLeftX + width <= D3D11_VIEWPORT_BOUNDS_MAX )
		{
			dxViewport.TopLeftX = topLeftX;
			dxViewport.Width    = width;
			
		} else
		{
			std::cout << "Warning (RenderWindow::addViewport): Viewport out of bounds. Clamping viewport." << std::endl;
			dxViewport.TopLeftX = 0.0f;
			dxViewport.Width    = D3D11_VIEWPORT_BOUNDS_MAX;
		}

		if ( height > 0 && topLeftY + height <= D3D11_VIEWPORT_BOUNDS_MAX )
		{
			dxViewport.TopLeftY = topLeftY;
			dxViewport.Height   = height;
			
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

	void RenderWindow::setClearColor( float red, float green, float blue, float alpha )
	{
		m_clearColor[0] = red;
		m_clearColor[1] = green;
		m_clearColor[2] = blue;
		m_clearColor[3] = alpha;
	}


	void RenderWindow::update()
	{
		if ( !m_isInit || !m_curViewport.cam )
			return;

		//static float lastTime = (float)timeGetTime();

		m_dxDevCont->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
		//TODO: make sure RSSetState and RSSetViewports are only called when they are really needed to be called
		m_dxDevCont->RSSetState(m_rasterizer);
		//TODO: add support for multiple viewports
		m_dxDevCont->RSSetViewports( 1, &m_curViewport.dxViewport );

		// clear the back buffer
		m_dxDevCont->ClearRenderTargetView( m_renderTargetView, m_clearColor );
		m_dxDevCont->ClearDepthStencilView( m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );


		//float currTime  = (float)timeGetTime();
		//float deltaTime = (currTime - lastTime) * 0.001f;
		//update( deltaTime );


		m_curViewport.cam->renderCurrentView();

		//mActiveCam->renderCurrentView();



		// flip the back buffer and the front buffer
		m_swapChain->Present(1, 0);
		//lastTime = currTime;
	}
}
