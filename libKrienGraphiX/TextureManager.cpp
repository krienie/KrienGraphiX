
#include <comdef.h>
#include <iostream>

#include "Texture.h"
#include "TextureManager.h"

namespace kgx
{
	TextureManager* TextureManager::m_inst = nullptr;

	TextureManager* TextureManager::getInst()
	{
		return m_inst;
	}

	void TextureManager::construct( _In_ ID3D11Device *m_dxDev )
	{
		if ( TextureManager::m_inst )
			delete TextureManager::m_inst;

		m_inst = new TextureManager( m_dxDev );
	}


	void TextureManager::destroy()
	{
		if ( TextureManager::m_inst )
			delete TextureManager::m_inst;
	}


	TextureManager::TextureManager( _In_ ID3D11Device *dxDevice )
		: m_dxDev(dxDevice), m_textures(), m_defaultSamp(nullptr)
	{
		// create default sampler
		m_defaultSamp = createSampler( D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, 4u );
	}

	TextureManager::~TextureManager()
	{
		clearTextureResources();
	}


	ID3D11SamplerState* TextureManager::getDefaultSampler() const
	{
		return m_defaultSamp;
	}

	void TextureManager::setDefaultFilter( const D3D11_FILTER &filter )
	{
		// release current sampler if present
		if ( m_defaultSamp )
			m_defaultSamp->Release();

		m_defaultSamp = createSampler( filter, D3D11_TEXTURE_ADDRESS_WRAP, 4u );
	}

	void TextureManager::setDefaultAddressMode( const D3D11_TEXTURE_ADDRESS_MODE &mode )
	{
		// release current sampler if present
		if ( m_defaultSamp )
			m_defaultSamp->Release();

		m_defaultSamp = createSampler( D3D11_FILTER_ANISOTROPIC, mode, 4u );
	}

	void TextureManager::setDefaultMaxAnisotropy( UINT val )
	{
		// release current sampler if present
		if ( m_defaultSamp )
			m_defaultSamp->Release();

		m_defaultSamp = createSampler( D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, val );
	}


	Texture* TextureManager::loadTexture( const std::string &filename )
	{
		return loadTexture( std::wstring(filename.begin(), filename.end()) );
	}
	Texture* TextureManager::loadTexture( const std::wstring &filename )
	{
		Texture *tex = getTexture( filename );
		if ( tex )
			return tex;

		tex = new Texture( filename, m_dxDev );
		m_textures.insert( std::pair<std::wstring, Texture*>(filename, tex) );

		return tex;
	}

	Texture* TextureManager::getTexture( const std::string &filename ) const
	{
		return getTexture( std::wstring(filename.cbegin(), filename.cend()) );
	}
	Texture* TextureManager::getTexture( const std::wstring &filename ) const
	{
		std::map<std::wstring, Texture*>::const_iterator it = m_textures.find( filename );
		if ( it != m_textures.cend() )
			return it->second;

		return nullptr;
	}

	void TextureManager::releaseTexture( const std::string &filename )
	{
		releaseTexture( std::wstring(filename.cbegin(), filename.cend()) );
	}
	void TextureManager::releaseTexture( const std::wstring &filename )
	{
		m_textures.erase( filename );
	}
	void TextureManager::releaseTexture( _In_ Texture* tex )
	{
		std::map<std::wstring, Texture*>::const_iterator it;

		for ( it = m_textures.begin(); it != m_textures.end(); ++it )
		{
			if ( it->second == tex )
			{
				m_textures.erase( it );
				return;
			}
		}
	}


	void TextureManager::clearTextureResources()
	{
		if ( m_defaultSamp )
		{
			m_defaultSamp->Release();
			m_defaultSamp = nullptr;
		}

		std::map<std::wstring, Texture*>::iterator it;
		for ( it = m_textures.begin(); it != m_textures.end(); ++it )
			delete it->second;
		m_textures.clear();
	}


	ID3D11SamplerState* TextureManager::createSampler( const D3D11_FILTER &filter, const D3D11_TEXTURE_ADDRESS_MODE &mode, UINT anisotropy ) const
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory( &sampDesc, sizeof( sampDesc ) );
		sampDesc.Filter         = filter;
		sampDesc.AddressU       = mode;
		sampDesc.AddressV       = mode;
		sampDesc.AddressW       = mode;
		sampDesc.MipLODBias     = 0.0f;
		sampDesc.MaxAnisotropy  = anisotropy;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		// borderColor stays at 0 (black with no alpha)
		sampDesc.MinLOD         = 0;
		sampDesc.MaxLOD         = D3D11_FLOAT32_MAX;

		ID3D11SamplerState* sampler = nullptr;
		HRESULT res = m_dxDev->CreateSamplerState( &sampDesc, &sampler );
		if ( FAILED( res ) )
		{
			_com_error error( res );
			std::cout << "Error (TextureManager::TextureManager): Error creating texture sampler:" << error.ErrorMessage() << std::endl;
			return nullptr;
		}

		return sampler;
	}
}
