#pragma once

#include "Rendering/Texture.h"

struct ID3D11RenderTargetView;

namespace kgx
{
    class RenderTexture : public Texture
    {
        public:
            RenderTexture( TextureID id, ID3D11Resource *buffer, ID3D11ShaderResourceView *shaderResourceView, ID3D11RenderTargetView *renderTargetView );
            virtual ~RenderTexture();

            ID3D11RenderTargetView* getRenderTargetView() const;

        private:
            ID3D11RenderTargetView *m_renderTargetView;

    };
}
