
#include <iostream>

#include "Camera.h"
#include "VertexInputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ShaderProgram.h"

namespace kgx
{
	ShaderProgram::ShaderProgram( ID3D11Device *dxDevice, ShaderProgramID id, const std::string &name )
		: Object(name), m_dxDev(dxDevice), m_dxDevCont(nullptr), m_progID(id), m_vertShader(nullptr),
		/*m_hullShader(nullptr), m_domainShader(nullptr), m_geomShader(nullptr),*/ m_pixShader(nullptr), m_constVarLinks()
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


	ShaderProgram::ShaderProgramID ShaderProgram::getID() const
	{
		return m_progID;
	}


	VertexShader* ShaderProgram::createVertexShader( const std::string &filename, const VertexInputLayout &layout )
	{
		m_vertShader = new VertexShader( m_dxDev, layout );

		if ( FAILED(m_vertShader->loadFromFile( std::wstring(filename.begin(), filename.end()) )) )
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
		m_pixShader = new PixelShader( m_dxDev );

		if ( FAILED(m_pixShader->loadFromFile( std::wstring(filename.begin(), filename.end()) )) )
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


	void ShaderProgram::addAutoShaderVar( ShaderType shaderType, const std::string &varName, ShaderAutoBindType varType )
	{
		ShaderBase *shader = getShader( shaderType );
		if ( shader )
			addAutoShaderVar( shader, varName, varType );
		else std::cout << "Error (ShaderProgram::addAutoShaderVar): Shader not found. Call ShaderProgram::createShadertypeShader() first." << std::endl;
	}
	void ShaderProgram::addAutoShaderVar( ShaderBase *shader, const std::string &varName, ShaderAutoBindType varType )
	{
		if ( !shader )
		{
			std::cout << "Error (ShaderProgram::addAutoShaderVar): Shader cannot be nullptr." << std::endl;
			return;
		}

		// get auto variable list or create one if not already present
		std::pair< std::map< ShaderBase*, std::vector<AutoShaderVar> >::iterator, bool > shaderVars;
		shaderVars = m_constVarLinks.insert( std::pair< ShaderBase*, std::vector<AutoShaderVar> >( shader, std::vector<AutoShaderVar>() ) );

		// add shader var to list
		shaderVars.first->second.push_back( AutoShaderVar(varName, varType) );
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

	void ShaderProgram::updateShaderVar( ShaderType shaderType, const std::string &name, const void *dataPtr )
	{
		ShaderBase *shader = getShader( shaderType );
		if ( shader )
			shader->updateConstantVariable( name, dataPtr );
		else std::cout << "Error (ShaderProgram::updateShaderVar): Shader not found. Call ShaderProgram::createShadertypeShader() first." << std::endl;
	}


	void ShaderProgram::activate( Camera *renderCam )
	{
		std::map< ShaderBase*, std::vector<AutoShaderVar> >::iterator shaderIt;
		std::vector<AutoShaderVar>::iterator varIt;

		// update shader constants and bind shaders to pipeline
		if ( m_vertShader )
		{
			shaderIt = m_constVarLinks.find(m_vertShader);
			if ( shaderIt != m_constVarLinks.end() )
				for ( varIt = shaderIt->second.begin(); varIt != shaderIt->second.end(); ++varIt )
					updateAutoShaderVar( renderCam, m_vertShader, *varIt );

			m_vertShader->activate();
		}
		if ( m_pixShader )
		{
			shaderIt = m_constVarLinks.find(m_pixShader);
			if ( shaderIt != m_constVarLinks.end() )
				for ( varIt = shaderIt->second.begin(); varIt != shaderIt->second.end(); ++varIt )
					updateAutoShaderVar( renderCam, m_pixShader, *varIt );

			m_pixShader->activate();
		}
	}


	void ShaderProgram::updateAutoShaderVar( Camera *renderCam, ShaderBase *shader, AutoShaderVar shaderVar )
	{
		float tempFloat = 0.0f;
		switch ( shaderVar.type )
		{
			case ShaderAutoBindType::CameraProjectionMatrix:
				shader->updateConstantVariable( shaderVar.name, &(renderCam->getProjMatrix().m[0]) );
				break;
			case ShaderAutoBindType::CameraViewMatrix:
				shader->updateConstantVariable( shaderVar.name, &(renderCam->getViewMatrix().m[0]) );
				break;
			case ShaderAutoBindType::CameraPosition:
				shader->updateConstantVariable( shaderVar.name, &(renderCam->getPosition()) );
				break;
			case ShaderAutoBindType::CameraTarget:
				shader->updateConstantVariable( shaderVar.name, &(renderCam->getTarget()) );
				break;
			case ShaderAutoBindType::CameraFieldOfView:
				tempFloat = renderCam->getFOV();
				shader->updateConstantVariable( shaderVar.name, &tempFloat );
				break;
			case ShaderAutoBindType::CameraAspectRatio:
				tempFloat = renderCam->getAspectRatio();
				shader->updateConstantVariable( shaderVar.name, &tempFloat );
				break;
			case ShaderAutoBindType::CameraNearZ:
				tempFloat = renderCam->getNearZ();
				shader->updateConstantVariable( shaderVar.name, &tempFloat );
				break;
			case ShaderAutoBindType::CameraFarZ:
				tempFloat = renderCam->getFarZ();
				shader->updateConstantVariable( shaderVar.name, &tempFloat );
				break;
			default:
				//This should never happen...
				std::cout << "(ShaderProgram::updateAutoShaderVar) Warning: Unknown shader variable type:" << shaderVar.type << " No update done." << std::endl;
				break;
		}
	}
}
