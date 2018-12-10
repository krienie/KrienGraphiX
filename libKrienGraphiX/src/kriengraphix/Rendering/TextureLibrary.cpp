
#include "Rendering/TextureLibrary.h"

#include <iostream>
#include <comdef.h>
#include <d3d11.h>

#include "Core/KGXCore.h"
#include "Rendering/Texture.h"
#include "Rendering/TextureLoader.h"
#include "Rendering/RenderTexture.h"

namespace kgx
{
	TextureLibrary::TextureLibrary()
		: m_dxDev(nullptr), m_dxDevCont(nullptr), m_nextTextureID(1),
			m_textures(), m_textureNames(), m_defaultSampler(nullptr)
	{
		m_dxDev = KGXCore::get()->getDxDevicePtr();
		m_dxDev->GetImmediateContext( &m_dxDevCont );

		m_defaultSampler = createSampler( D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, 4u );
	}

	TextureLibrary::~TextureLibrary()
	{
		clear();

		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}

	void TextureLibrary::clear()
	{
		if ( m_defaultSampler )
		{
			m_defaultSampler->Release();
			m_defaultSampler = nullptr;
		}

		for ( auto &textureIDPair : m_textures )
			delete textureIDPair.second;

		m_textures.clear();
		m_textureNames.clear();
	}

	ID3D11SamplerState* TextureLibrary::getDefaultSampler() const
	{
		return m_defaultSampler;
	}

	Texture* TextureLibrary::getTexture( const std::string &textureName ) const
	{
		std::unordered_map<std::string, Texture::TextureID>::const_iterator it;
		it = m_textureNames.find( textureName );

		if ( it != m_textureNames.cend() )
			return getTexture( it->second );

		return nullptr;
	}
	Texture* TextureLibrary::getTexture( Texture::TextureID id ) const
	{
		std::unordered_map<Texture::TextureID, Texture*>::const_iterator it = m_textures.find( id );
		if ( it != m_textures.cend() )
			return it->second;

		return nullptr;
	}

	Texture* TextureLibrary::loadFromDisk( const std::string &file )
	{
		Texture* texture = getTexture( file );
		if ( texture )
			return texture;

		ID3D11Resource *buffer = nullptr;
		ID3D11ShaderResourceView *shaderResourceView = nullptr;
		HRESULT res = TextureLoader::loadAsShaderResource( std::wstring( file.begin(), file.end() ), &buffer, &shaderResourceView, m_dxDev, m_dxDevCont );
		if ( FAILED(res) )
			return nullptr;

		Texture *newTexture = new Texture(m_nextTextureID, buffer, shaderResourceView);
		m_textures.insert( std::make_pair(m_nextTextureID, newTexture) );
		m_textureNames.insert( std::make_pair(file, m_nextTextureID) );

		++m_nextTextureID;

		return newTexture;
	}

	RenderTexture* TextureLibrary::createRenderTexture( unsigned int width, unsigned int height )
	{
		// Create texture
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width              = width;
		texDesc.Height             = height;
		texDesc.MipLevels          = 1;
		texDesc.ArraySize          = 1;
		texDesc.Format             = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.SampleDesc.Count   = 1;
		texDesc.SampleDesc.Quality = 0; //D3D11_STANDARD_MULTISAMPLE_PATTERN;
		texDesc.Usage              = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texDesc.CPUAccessFlags     = 0;
		texDesc.MiscFlags          = 0;

		ID3D11Texture2D *texture = nullptr;
		HRESULT res = m_dxDev->CreateTexture2D( &texDesc, nullptr, &texture );
		if ( FAILED(res) )
			return nullptr;

		// Setup the description of the render target view.
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format             = texDesc.Format;
		rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		// Create the render target view
		ID3D11RenderTargetView *renderTargetView;
		res = m_dxDev->CreateRenderTargetView( texture, &rtvDesc, &renderTargetView );
		if ( FAILED( res ) )
		{
			texture->Release();
			return nullptr;
		}

		// Setup the description of the shader resource view.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format                    = texDesc.Format;
		srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels       = texDesc.MipLevels;

		// Create the shader resource view.
		ID3D11ShaderResourceView *shaderResourceView;
		res = m_dxDev->CreateShaderResourceView( texture, &srvDesc, &shaderResourceView );
		if ( FAILED(res) )
		{
			renderTargetView->Release();
			texture->Release();
			return nullptr;
		}

		ID3D11Resource *texBuffer = texture;
		RenderTexture *newTexture = new RenderTexture( m_nextTextureID, texBuffer, shaderResourceView, renderTargetView );
		m_textures.insert( std::make_pair( m_nextTextureID, newTexture ) );
		++m_nextTextureID;

		return newTexture;
	}

	ID3D11SamplerState * TextureLibrary::createSampler( const D3D11_FILTER &filter, const D3D11_TEXTURE_ADDRESS_MODE &mode, UINT anisotropy ) const
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory( &sampDesc, sizeof( sampDesc ) );
		sampDesc.Filter = filter;
		sampDesc.AddressU = mode;
		sampDesc.AddressV = mode;
		sampDesc.AddressW = mode;
		sampDesc.MipLODBias = 0.0f;
		sampDesc.MaxAnisotropy = anisotropy;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		// borderColor stays at 0 (black with no alpha)
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		ID3D11SamplerState* sampler = nullptr;
		HRESULT res = m_dxDev->CreateSamplerState( &sampDesc, &sampler );
		if ( FAILED( res ) )
		{
			_com_error error( res );
			std::cout << "Error (TextureLibrary::createSampler): Error creating texture sampler:" << error.ErrorMessage() << std::endl;
			return nullptr;
		}

		return sampler;
	}

	void TextureLibrary::getShaderResourceViews(std::vector<ID3D11ShaderResourceView*>& srvList) const
	{
		srvList.clear();
		srvList.reserve( m_textures.size() );

		for ( auto &texture : m_textures )
			srvList.push_back( texture.second->getResourceView() );
	}
}
