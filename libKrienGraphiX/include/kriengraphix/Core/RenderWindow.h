
#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>

namespace kgx
{
	class Camera;

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
			unsigned int getWidth() const;
			unsigned int getHeight() const;

			void update();

		private:
			// no copying allowed
			RenderWindow( const RenderWindow& )            = delete;
			RenderWindow& operator=( const RenderWindow& ) = delete;

			struct Viewport
			{
				Viewport() : dxViewport(), cam(nullptr) {}
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
