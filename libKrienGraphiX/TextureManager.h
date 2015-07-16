
#pragma once

#include <d3d11.h>
#include <map>

class Texture;

namespace kgx
{
	class TextureManager
	{
	public:
		static TextureManager* getInst();
		static void construct( _In_ ID3D11Device *dxDevice );
		static void destroy();

		//TODO: add default search path(s)

		ID3D11SamplerState* getDefaultSampler() const;
		void setDefaultFilter( const D3D11_FILTER &filter );
		void setDefaultAddressMode( const D3D11_TEXTURE_ADDRESS_MODE &mode );
		void setDefaultMaxAnisotropy( UINT val );


		Texture* loadTexture( const std::string &filename );
		Texture* loadTexture( const std::wstring &filename );

		Texture* getTexture( const std::string &filename ) const;
		Texture* getTexture( const std::wstring &filename ) const;

		void releaseTexture( const std::string &filename );
		void releaseTexture( const std::wstring &filename );
		void releaseTexture( _In_ Texture* tex );

	private:
		explicit TextureManager( _In_ ID3D11Device *dxDevice );
		~TextureManager();
		// no copying allowed
		TextureManager( const TextureManager& );
		TextureManager& operator=(const TextureManager&);

		ID3D11SamplerState* createSampler( const D3D11_FILTER &filter, const D3D11_TEXTURE_ADDRESS_MODE &mode, UINT anisotropy ) const;

		static TextureManager* m_inst;

		ID3D11Device *m_dxDev;

		std::map<std::wstring, Texture*> m_textures;
		ID3D11SamplerState *m_defaultSamp;
	};
}
