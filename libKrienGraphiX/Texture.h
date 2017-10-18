
#pragma once

struct ID3D11Resource;
struct ID3D11ShaderResourceView;

namespace kgx
{
	class Texture
	{
		public:
			typedef int TextureID;

			Texture( TextureID id, ID3D11Resource *buffer, ID3D11ShaderResourceView *shaderResourceView );
			Texture( const Texture &other ) = delete;
			virtual ~Texture();

			TextureID getID() const;
			ID3D11Resource* getBuffer() const;
			ID3D11ShaderResourceView* getResourceView() const;

			Texture& operator=(const Texture &rhs) = delete;

		private:
			TextureID m_id;

			ID3D11Resource *m_buffer;
			ID3D11ShaderResourceView *m_shaderView;

	};
}
