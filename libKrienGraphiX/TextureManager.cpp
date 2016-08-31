
#include <comdef.h>
#include <iostream>

#include <boost/filesystem.hpp>

#include "Filesystem.h"
#include "Texture.h"
#include "TextureManager.h"

namespace kgx
{
	TextureManager* TextureManager::m_inst = nullptr;

	TextureManager* TextureManager::getInst()
	{
		return m_inst;
	}

	void TextureManager::construct( ID3D11Device *m_dxDev )
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


	TextureManager::TextureManager( ID3D11Device *dxDevice )
		: m_dxDev(dxDevice), m_nextTextureID(1u), m_textures(), m_defaultSamp(nullptr)
	{
	}

	TextureManager::~TextureManager()
	{
		clearTextureResources();
	}


	ID3D11SamplerState* TextureManager::getDefaultSampler()
	{
		// create default sampler
		if ( !m_defaultSamp )
			m_defaultSamp = createSampler( D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, 4u );

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


	Texture::TextureID TextureManager::loadTexture( const std::string &filename )
	{
		// check if file exists, if not: abort
		std::string absTexFile = filename;
		if ( !boost::filesystem::path(filename).is_absolute() )
			absTexFile = filesystem::getAbsolutePath(filename);
		if ( absTexFile.size() == 0 )
		{
			std::cout << "Error (TextureManager::loadTexture): Texture source file not specified." << std::endl;
			return 0;
		}

		if ( !boost::filesystem::exists( absTexFile ) )
		{
			std::cout << "Error (TextureManager::loadTexture): Texture source file does not exist." << std::endl;
			return 0;
		}

		// check if the requested texture is already loaded
		std::map<Texture::TextureID, Texture*>::const_iterator it;
		for ( it = m_textures.begin(); it != m_textures.end(); ++it )
			if ( it->second->getFileName() == filename )
				return it->first;

		Texture *tex = new Texture( absTexFile, m_nextTextureID, m_dxDev );
		m_textures.insert( std::pair<Texture::TextureID, Texture*>(m_nextTextureID, tex) );
		++m_nextTextureID;

		return m_nextTextureID - 1u;
	}

	Texture* TextureManager::getTexture( Texture::TextureID id ) const
	{
		std::map<Texture::TextureID, Texture*>::const_iterator it = m_textures.find( id );
		if ( it != m_textures.cend() )
			return it->second;

		return nullptr;
	}

	void TextureManager::releaseTexture( Texture::TextureID id )
	{
		m_textures.erase( id );
	}
	void TextureManager::releaseTexture( Texture* tex )
	{
		std::map<Texture::TextureID, Texture*>::const_iterator it;

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

		std::map<Texture::TextureID, Texture*>::iterator it;
		for ( it = m_textures.begin(); it != m_textures.end(); ++it )
			delete it->second;
		m_textures.clear();

		m_nextTextureID = 1u;
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
