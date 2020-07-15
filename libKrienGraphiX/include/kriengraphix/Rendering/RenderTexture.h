#pragma once

#include <d3d11.h>

#include "Rendering/Texture.h"

struct ID3D11RenderTargetView;

namespace kgx
{
class RenderTexture final : public Texture
{
    public:
        //RenderTexture( TextureID id, ID3D11Resource *buffer, ID3D11ShaderResourceView *shaderResourceView, ID3D11RenderTargetView *renderTargetView );
        RenderTexture(TextureID id, unsigned int width, unsigned int height);
        virtual ~RenderTexture();

        ID3D11RenderTargetView* getRenderTargetView() const;

        [[nodiscard]]
        unsigned int getWidth() const;
        [[nodiscard]]
        unsigned int getHeight() const;

    private:
        bool createDxRenderTexture(unsigned int width, unsigned int height, ID3D11Resource **buff, ID3D11ShaderResourceView **srv, ID3D11RenderTargetView **rtv);

        ID3D11Device *m_dxDev;
        ID3D11DeviceContext *m_dxDevCont;

        ID3D11RenderTargetView *m_renderTargetView;

        unsigned int m_width;
        unsigned int m_height;

};
}
