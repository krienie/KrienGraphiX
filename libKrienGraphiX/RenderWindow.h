
#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include <vector>

#include "Camera.h"

namespace kgx
{
	//TODO: voorlopig een renderWindow => later kan dit nog een renderTexture of wat dan ook worden
	class RenderWindow
	{
		public:
			RenderWindow( _In_ ID3D11Device *dxDevice, _In_ IDXGIFactory2 *dxgiFactory_ );
			~RenderWindow();

			bool create( HWND windowHandle );
			void setViewport( _In_ Camera *cam, float topLeftX = 0, float topLeftY = 0, float width = -1.0f, float height = -1.0f );
			void update();

		private:
			struct Viewport
			{
				Viewport() : dxViewport(), cam(0) {}
				Viewport( D3D11_VIEWPORT dxView, Camera *camera ) : dxViewport(dxView), cam(camera) { }

				D3D11_VIEWPORT dxViewport;
				Camera *cam;
			};

			ID3D11Device *dxDev;
			ID3D11DeviceContext *dxDevCont;
			IDXGIFactory2 *dxgiFactory;

			IDXGISwapChain1 *swapChain;
			ID3D11DepthStencilView *depthStencilView;
			ID3D11RenderTargetView *renderTargetView;
			ID3D11RasterizerState *rasterizer;

			//std::vector<Viewport> viewports;
			Viewport curViewport;

			UINT backBuffWidth;
			UINT backBuffHeight;

			bool isInit;
	};
}
