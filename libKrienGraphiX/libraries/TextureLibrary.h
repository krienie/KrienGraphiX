
#pragma once

#include <unordered_map>
#include <d3d11.h>

#include "../Texture.h"

namespace kgx
{
	class RenderTexture;

	class TextureLibrary
	{
		public:
			TextureLibrary();
			~TextureLibrary();

			void clear();
			ID3D11SamplerState* getDefaultSampler() const;
			Texture* getTexture( const std::string &textureName ) const;
			Texture* getTexture( Texture::TextureID id ) const;
			Texture* loadFromDisk( const std::string &file );
			RenderTexture* createRenderTexture( unsigned int width, unsigned int height );
			
			void getShaderResourceViews( std::vector<ID3D11ShaderResourceView*> &srvList ) const;

		private:
			ID3D11SamplerState* createSampler( const D3D11_FILTER &filter, const D3D11_TEXTURE_ADDRESS_MODE &mode, UINT anisotropy ) const;

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			Texture::TextureID m_nextTextureID;
			std::unordered_map<Texture::TextureID, Texture*> m_textures;
			std::unordered_map<std::string, Texture::TextureID> m_textureNames;

			ID3D11SamplerState *m_defaultSampler;
	};
}
