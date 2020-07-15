
#pragma once

#include <d3d11.h>

namespace kgx
{
class CameraComponent;
class RenderWindow;

class SceneView final
{
    public:
        SceneView(RenderWindow *parentWindow);
        ~SceneView();

        [[nodiscard]]
        D3D11_VIEWPORT getViewport() const;
        void setViewport(float topLeftX = 0, float topLeftY = 0, float width = -1.0f, float height = -1.0f);

        [[nodiscard]]
        ID3D11RenderTargetView* getRenderTarget() const;
        [[nodiscard]]
        ID3D11DepthStencilView* getDepthStencil() const;
        [[nodiscard]]
        ID3D11RasterizerState* getRasterizerState() const;
        [[nodiscard]]
        float getWidth() const;
        [[nodiscard]]
        float getHeight() const;

        void clear();
        void present();

    private:
        RenderWindow *m_parentWindow;

        ID3D11DeviceContext *m_dxDevCont;
        D3D11_VIEWPORT m_dxViewport;
        ID3D11RasterizerState* m_rasterizer;
};
}