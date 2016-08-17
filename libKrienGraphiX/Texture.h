
#pragma once

#include <string>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Resource;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;

namespace kgx
{
	class Texture
	{
		public:
			typedef INT TextureID;

			Texture( const std::string &file, TextureID id, ID3D11Device *dxDevice );
			explicit Texture( const Texture &other );
			~Texture();

			TextureID getID() const;

			std::string getFileName() const;
			ID3D11Resource* getBuffer() const;
			ID3D11ShaderResourceView* getResourceView() const;

			Texture& operator=(const Texture &rhs);

		private:
			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			TextureID m_textureID;

			ID3D11Resource *m_buffer;
			ID3D11ShaderResourceView *m_shaderView;

			std::string m_filename;
	};
}
