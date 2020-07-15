
#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>

namespace kgx
{
    class RenderWindow
    {
        public:
            RenderWindow( ID3D11Device *dxDevice, IDXGIFactory2 *dxgiFactory );
            ~RenderWindow();
            RenderWindow(const RenderWindow&) noexcept             = delete;
            RenderWindow& operator=(const RenderWindow& ) noexcept = delete;
            RenderWindow(RenderWindow&&) noexcept                  = delete;
            RenderWindow& operator=(RenderWindow&&) noexcept       = delete;

            bool create( HWND windowHandle );

            void setClearColor( float red, float green, float blue );

            [[nodiscard]]
            bool isFullscreen() const;
            void setFullscreen( bool active );

            [[nodiscard]]
            float getAspectRatio() const;
            [[nodiscard]]
            unsigned int getWidth() const;
            [[nodiscard]]
            unsigned int getHeight() const;

            [[nodiscard]]
            ID3D11RenderTargetView* getRTV() const;
            [[nodiscard]]
            ID3D11DepthStencilView* getDSV() const;

            void clear();
            void present();

        private:
            ID3D11Device *m_dxDev;
            ID3D11DeviceContext *m_dxDevCont;
            IDXGIFactory2 *m_dxgiFactory;
            IDXGISwapChain1 *m_swapChain;
            ID3D11RenderTargetView *m_renderTargetView;
            ID3D11DepthStencilView *m_depthStencilView;

            UINT m_backBuffWidth;
            UINT m_backBuffHeight;

            bool m_isInit;

            float m_clearColor[4];
    };
}
