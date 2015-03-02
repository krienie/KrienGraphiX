
#include <iostream>

#include "RenderWindow.h"

namespace kgx
{
	RenderWindow::RenderWindow( _In_ ID3D11Device *dxDevice, _In_ IDXGIFactory2 *dxgiFactory_ )
		: dxDev(dxDevice), dxDevCont(0), dxgiFactory(dxgiFactory_), swapChain(0), depthStencilView(0),
			renderTargetView(0), rasterizer(0), curViewport(), backBuffWidth(0U), backBuffHeight(0U), isInit(false)
	{
		dxDev->GetImmediateContext( &dxDevCont );
	}

	RenderWindow::~RenderWindow()
	{
		if ( rasterizer )
			rasterizer->Release();

		if ( renderTargetView )
			renderTargetView->Release();

		if ( depthStencilView )
			depthStencilView->Release();

		if ( swapChain )
			swapChain->Release();

		if ( dxDevCont )
			dxDevCont->Release();
	}


	bool RenderWindow::create( HWND windowHandle )
	{
		// create swapChain descriptor
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
		HRESULT res = dxgiFactory->CreateSwapChainForHwnd( dxDev, windowHandle, &swapDesc, NULL, NULL, &swapChain );

		if ( FAILED(res) )
		{
			//TODO: add better error description here
			std::cout << "Error (RenderWindow::create): Error creating swapChain." << std::endl;
			return false;
		}


		// create and initialize backbuffer
		ID3D11Texture2D *backBuff;
		swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuff));
		D3D11_TEXTURE2D_DESC backBufferDesc;
		backBuff->GetDesc(&backBufferDesc);

		backBuffWidth  = backBufferDesc.Width;
		backBuffHeight = backBufferDesc.Height;

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

		dxDev->CreateTexture2D( &depthBuffDesc, NULL, &depthBuff );
		res = dxDev->CreateDepthStencilView( depthBuff, NULL, &depthStencilView );
		depthBuff->Release();

		if ( FAILED(res) )
		{
			backBuff->Release();
			std::cout << "Error (RenderWindow::create): Error creating DepthStencilView." << std::endl;
			return false;
		}
		

		// create renderTarget from the backBuffer and depth stencil view
		res = dxDev->CreateRenderTargetView(backBuff, NULL, &renderTargetView);
		backBuff->Release();

		if ( FAILED(res) )
		{
			std::cout << "Error (RenderWindow::create): Error creating RenderTargetView." << std::endl;
			return false;
		}

		// Setup the rasterizer
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode				 = D3D11_CULL_BACK;			//D3D11_CULL_FRONT, D3D11_CULL_BACK or D3D11_CULL_NONE
		rasterDesc.DepthBias			 = 0;
		rasterDesc.DepthBiasClamp		 = 0.0f;
		rasterDesc.DepthClipEnable		 = true;
		rasterDesc.FillMode				 = D3D11_FILL_SOLID;	//D3D11_FILL_WIREFRAME or D3D11_FILL_SOLID
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable	 = false;
		rasterDesc.ScissorEnable		 = false;
		rasterDesc.SlopeScaledDepthBias  = 0.0f;

		res = dxDev->CreateRasterizerState(&rasterDesc, &rasterizer);
		if ( FAILED(res) )
		{
			std::cout << "Error (RenderWindow::create): Error creating Rasterizer state." << std::endl;
			return false;
		}

		isInit = true;
		return true;
	}


	void RenderWindow::setViewport( _In_ Camera *cam, float topLeftX, float topLeftY, float width, float height )
	{
		// NOTE: no warning is given when this function is called when the RenderWindow is not initialized

		// initialize viewport
		D3D11_VIEWPORT dxViewport;
		ZeroMemory(&dxViewport, sizeof(D3D11_VIEWPORT));

		if ( topLeftX + width <= D3D11_VIEWPORT_BOUNDS_MAX )
		{
			dxViewport.TopLeftX = topLeftX;

			if ( width < 0.0f )
				dxViewport.Width = static_cast<float>(backBuffWidth);
			else dxViewport.Width = width;
			
		} else
		{
			std::cout << "Warning (RenderWindow::addViewport): Viewport out of bounds. Using default settings." << std::endl;
			dxViewport.TopLeftX = 0.0f;
			dxViewport.Width    = D3D11_VIEWPORT_BOUNDS_MAX;
		}

		if ( topLeftY + height <= D3D11_VIEWPORT_BOUNDS_MAX )
		{
			dxViewport.TopLeftY = topLeftY;

			if ( height < 0.0f )
				dxViewport.Height = static_cast<float>(backBuffHeight);
			else dxViewport.Height = height;
			
		} else
		{
			std::cout << "Warning (RenderWindow::addViewport): Viewport out of bounds. Using default settings." << std::endl;
			dxViewport.TopLeftY = 0.0f;
			dxViewport.Height   = D3D11_VIEWPORT_BOUNDS_MAX;
		}

		dxViewport.MinDepth = D3D11_MIN_DEPTH;
		dxViewport.MaxDepth = D3D11_MAX_DEPTH;

		//viewports.push_back( Viewport(dxViewport, cam) );
		curViewport = Viewport(dxViewport, cam);
	}


	void RenderWindow::update()
	{
		if ( !isInit || !curViewport.cam )
			return;

		//static float lastTime = (float)timeGetTime();

		dxDevCont->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		//TODO: make sure RSSetState and RSSetViewports are only called when they are really needed to be called
		dxDevCont->RSSetState(rasterizer);
		//TODO: add support for multiple viewports
		dxDevCont->RSSetViewports( 1, &curViewport.dxViewport );

		// clear the back buffer to grey
		//TODO: create something so that the user can change the clearcolor
		const float clearColor[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
		dxDevCont->ClearRenderTargetView( renderTargetView, clearColor );
		dxDevCont->ClearDepthStencilView( depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );


		//float currTime  = (float)timeGetTime();
		//float deltaTime = (currTime - lastTime) * 0.001f;
		//update( deltaTime );


		curViewport.cam->renderCurrentView();

		//mActiveCam->renderCurrentView();



		// flip the back buffer and the front buffer
		swapChain->Present(1, 0);
		//lastTime = currTime;
	}
}
