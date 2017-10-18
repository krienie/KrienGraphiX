
#include <d3d11.h>

#include "Texture.h"

namespace kgx
{
	Texture::Texture( TextureID id, ID3D11Resource *buffer, ID3D11ShaderResourceView *shaderResourceView )
		: m_id(id), m_buffer(buffer), m_shaderView(shaderResourceView)
	{
	}

	Texture::~Texture()
	{
		if ( m_shaderView )
			m_shaderView->Release();

		if ( m_buffer )
			m_buffer->Release();
	}

	Texture::TextureID Texture::getID() const
	{
		return m_id;
	}

	ID3D11Resource* Texture::getBuffer() const
	{
		return m_buffer;
	}

	ID3D11ShaderResourceView* Texture::getResourceView() const
	{
		return m_shaderView;
	}
}
