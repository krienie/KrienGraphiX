
#include <d3d11.h>

#include "RenderTexture.h"

namespace kgx
{
	RenderTexture::RenderTexture( TextureID id, ID3D11Resource *buffer, ID3D11ShaderResourceView *shaderResourceView,
									   ID3D11RenderTargetView *renderTargetView )
		: Texture(id, buffer, shaderResourceView), m_renderTargetView(renderTargetView)
	{
	}

	RenderTexture::~RenderTexture()
	{
		if ( m_renderTargetView )
			m_renderTargetView->Release();
	}

	ID3D11RenderTargetView* RenderTexture::getRenderTargetView() const
	{
		return m_renderTargetView;
	}
}
