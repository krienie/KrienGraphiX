
#include <iostream>

#include "VertexInputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ShaderProgram.h"

namespace kgx
{
	ShaderProgram::ShaderProgram( ID3D11Device *dxDevice, ShaderProgramID id, const std::string &name )
		: m_dxDev(dxDevice), m_dxDevCont(nullptr), m_name(name), m_progID(id), m_vertShader(nullptr),
		/*m_hullShader(nullptr), m_domainShader(nullptr), m_geomShader(nullptr),*/ m_pixShader(nullptr)
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );
	}

	ShaderProgram::~ShaderProgram()
	{
		if ( m_vertShader )
			delete m_vertShader;
		/*if ( m_hullShader )
			delete m_hullShader;
		if ( m_domainShader )
			delete m_domainShader;
		if ( m_geomShader )
			delete m_geomShader;*/
		if ( m_pixShader )
			delete m_pixShader;
		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}


	std::string ShaderProgram::getName() const
	{
		return m_name;
	}

	ShaderProgram::ShaderProgramID ShaderProgram::getID() const
	{
		return m_progID;
	}


	VertexShader* ShaderProgram::createVertexShader( const std::string &filename, const VertexInputLayout &layout )
	{
		if ( m_vertShader )
			delete m_vertShader;

		m_vertShader = new VertexShader( m_dxDev, layout );

		if ( FAILED(m_vertShader->loadFromFile( filename )) )
		{
			// shader loading failed
			std::cout << "Error (ShaderProgram::createVertexShader): Error creating vertex shader" << std::endl;
			delete m_vertShader;
			m_vertShader = nullptr;
		}

		return m_vertShader;
	}

	PixelShader* ShaderProgram::createPixelShader( const std::string &filename )
	{
		if ( m_pixShader )
			delete m_pixShader;

		m_pixShader = new PixelShader( m_dxDev );

		if ( FAILED(m_pixShader->loadFromFile( filename )) )
		{
			// shader loading failed
			std::cout << "Error (ShaderProgram::createPixelShader): Error creating pixel shader" << std::endl;
			delete m_pixShader;
			m_pixShader = nullptr;
		}

		return m_pixShader;
	}


	VertexShader* ShaderProgram::getVertexShader() const
	{
		return m_vertShader;
	}

	PixelShader* ShaderProgram::getPixelShader() const
	{
		return m_pixShader;
	}

	ShaderBase* ShaderProgram::getShader( ShaderType type ) const
	{
		switch ( type )
		{
			case ShaderType::Vertex:
				return m_vertShader;
				break;
				/*case ShaderType::Hull:
				std::cout << "Warning (ShaderProgram::getShader): Hull shader not supported yet." << std::endl;
				return nullptr;
				//return m_hullShader;
				break;
				case ShaderType::Domain:
				std::cout << "Warning (ShaderProgram::getShader): Domain shader not supported yet." << std::endl;
				return nullptr;
				//return m_domainShader;
				break;
				case ShaderType::Geometry:
				std::cout << "Warning (ShaderProgram::getShader): Geometry shader not supported yet." << std::endl;
				return nullptr;
				//return m_geomShader;
				break;*/
			case ShaderType::Pixel:
				return m_pixShader;
				break;
			default:
				return nullptr;
		}
	}


	void ShaderProgram::activate()
	{
		activate( m_dxDevCont );
	}
	void ShaderProgram::activate( ID3D11DeviceContext *dxContext )
	{
		// bind shaders to pipeline
		if ( m_vertShader )
			m_vertShader->activate( dxContext );
		else dxContext->VSSetShader( nullptr, 0, 0 );		// disable vertex shader

		if ( m_pixShader )
			m_pixShader->activate( dxContext );
		else dxContext->PSSetShader( nullptr, 0, 0 );		// disable pixel shader
	}
}
