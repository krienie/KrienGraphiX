
#include <d3d11.h>
#include <iostream>

#include "TextureLoader.h"
#include "Texture.h"

namespace kgx
{
	Texture::Texture( const std::string &file, ID3D11Device *dxDevice )
		: m_dxDev(dxDevice), m_dxDevCont(nullptr), m_buffer(nullptr), m_shaderView(nullptr),
			m_filename(file)
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );

		HRESULT res = TextureLoader::loadAsShaderResource( std::wstring(file.begin(), file.end()), &m_buffer, &m_shaderView, m_dxDev, m_dxDevCont );

		if ( FAILED(res) )
		{
			//TODO: maybe change this a bit to allow texture loading failures..
			std::cout << "Error (Texture::Texture): Error loading texture " << file.c_str() << std::endl;
			abort();
		}
	}

	Texture::Texture( const Texture &other )
		: m_dxDev(other.m_dxDev), m_dxDevCont(other.m_dxDevCont), m_buffer(other.m_buffer), m_shaderView(other.m_shaderView),
			m_filename(other.m_filename)
	{
		if ( m_dxDevCont )
			m_dxDev->GetImmediateContext( &m_dxDevCont );

		if ( m_buffer )
			m_buffer->AddRef();
		if ( m_shaderView )
			m_shaderView->AddRef();
	}

	Texture::~Texture()
	{
		if ( m_shaderView )
			m_shaderView->Release();
		if ( m_buffer )
			m_buffer->Release();
		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}

	std::string Texture::getName() const
	{
		return m_filename;
	}

	ID3D11Resource* Texture::getBuffer() const
	{
		return m_buffer;
	}

	ID3D11ShaderResourceView* Texture::getResourceView() const
	{
		return m_shaderView;
	}


	Texture& Texture::operator=( const Texture &rhs )
	{
		m_dxDev      = rhs.m_dxDev;
		m_dxDevCont  = rhs.m_dxDevCont;
		m_buffer     = rhs.m_buffer;
		m_shaderView = rhs.m_shaderView;
		m_filename   = rhs.m_filename;

		if ( m_dxDevCont )
			m_dxDev->GetImmediateContext( &m_dxDevCont );

		if ( m_buffer )
			m_buffer->AddRef();
		if ( m_shaderView )
			m_shaderView->AddRef();

		return *this;
	}
}
