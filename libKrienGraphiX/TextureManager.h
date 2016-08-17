
#pragma once

#include <d3d11.h>
#include <map>

#include "Texture.h"

namespace kgx
{
	class TextureManager
	{
	public:
		static TextureManager* getInst();
		static void construct( ID3D11Device *dxDevice );
		static void destroy();

		//TODO: add default search path(s)

		ID3D11SamplerState* getDefaultSampler();
		void setDefaultFilter( const D3D11_FILTER &filter );
		void setDefaultAddressMode( const D3D11_TEXTURE_ADDRESS_MODE &mode );
		void setDefaultMaxAnisotropy( UINT val );

		Texture::TextureID loadTexture( const std::string &filename );
		Texture* getTexture( Texture::TextureID id ) const;

		void releaseTexture( Texture::TextureID id );
		void releaseTexture( Texture* tex );


		void clearTextureResources();

	private:
		explicit TextureManager( ID3D11Device *dxDevice );
		~TextureManager();
		// no copying allowed
		TextureManager( const TextureManager& );
		TextureManager& operator=(const TextureManager&);

		ID3D11SamplerState* createSampler( const D3D11_FILTER &filter, const D3D11_TEXTURE_ADDRESS_MODE &mode, UINT anisotropy ) const;

		static TextureManager* m_inst;

		ID3D11Device *m_dxDev;

		Texture::TextureID m_nextTextureID;
		std::map<Texture::TextureID, Texture*> m_textures;
		ID3D11SamplerState *m_defaultSamp;
	};
}
