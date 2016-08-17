
#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include <vector>

namespace kgx
{
	class Camera;

	//TODO: voorlopig een renderWindow => later kan dit nog een renderTexture of wat dan ook worden
	class RenderWindow
	{
		public:
			RenderWindow( ID3D11Device *dxDevice, IDXGIFactory2 *dxgiFactory );
			~RenderWindow();

			bool create( HWND windowHandle );

			void setViewport( Camera *cam, float topLeftX = 0, float topLeftY = 0, float width = -1.0f, float height = -1.0f );
			void setClearColor( float red, float green, float blue );

			bool isFullscreen() const;
			void setFullscreen( bool active );

			float getAspectRatio() const;

			void update();

		private:
			// no copying allowed
			RenderWindow( const RenderWindow& );
			RenderWindow& operator=( const RenderWindow& );

			//void enumAdaptersTest();

			struct Viewport
			{
				Viewport() : dxViewport(), cam(0) {}
				Viewport( D3D11_VIEWPORT dxView, Camera *camera ) : dxViewport(dxView), cam(camera) { }

				D3D11_VIEWPORT dxViewport;
				Camera *cam;
			};

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;
			IDXGIFactory2 *m_dxgiFactory;
			IDXGISwapChain1 *m_swapChain;
			ID3D11RenderTargetView *m_renderTargetView;
			ID3D11DepthStencilView *m_depthStencilView;
			ID3D11RasterizerState *m_rasterizer;

			Viewport m_curViewport;

			UINT m_backBuffWidth;
			UINT m_backBuffHeight;

			bool m_isInit;

			float m_clearColor[4];
	};
}
