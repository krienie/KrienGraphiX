
#include <iostream>

#include "Camera.h"
#include "VertexInputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "RenderableObject.h"
#include "ShaderProgram.h"

namespace kgx
{
	ShaderProgram::ShaderProgram( _In_ ID3D11Device *dxDevice, ShaderProgramID id )
		: m_dxDev( dxDevice ), m_dxDevCont( nullptr ), m_progID(id), m_vertShader( nullptr ),
			m_pixShader(nullptr), m_constVarLinks()
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );
	}

	ShaderProgram::~ShaderProgram()
	{
		if ( m_vertShader )
			delete m_vertShader;
		if ( m_pixShader )
			delete m_pixShader;
		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}


	ShaderProgram::ShaderProgramID ShaderProgram::getID() const
	{
		return m_progID;
	}


	VertexShader* ShaderProgram::createVertexShader( const std::wstring &filename, const VertexInputLayout &layout )
	{
		m_vertShader = new VertexShader( m_dxDev, layout );

		if ( FAILED(m_vertShader->loadFromFile(filename)) )
		{
			// shader loading failed
			std::cout << "Error (ShaderProgram::createVertexShader): Error creating vertex shader" << std::endl;
			delete m_vertShader;
			m_vertShader = nullptr;
		}

		return m_vertShader;
	}

	VertexShader* ShaderProgram::createVertexShader( const std::string &filename, const VertexInputLayout &layout )
	{
		std::wstring wFilename( filename.begin(), filename.end() );
		return createVertexShader( wFilename, layout );
	}

	PixelShader* ShaderProgram::createPixelShader( const std::wstring &filename )
	{
		m_pixShader = new PixelShader( m_dxDev );

		if ( FAILED(m_pixShader->loadFromFile(filename)) )
		{
			// shader loading failed
			std::cout << "Error (ShaderProgram::createPixelShader): Error creating pixel shader" << std::endl;
			delete m_pixShader;
			m_pixShader = nullptr;
		}

		return m_pixShader;
	}

	PixelShader* ShaderProgram::createPixelShader( const std::string &filename )
	{
		std::wstring wFilename( filename.begin(), filename.end() );
		return createPixelShader( wFilename );
	}


	VertexShader* ShaderProgram::getVertexShader() const
	{
		return m_vertShader;
	}

	PixelShader* ShaderProgram::getPixelShader() const
	{
		return m_pixShader;
	}


	void ShaderProgram::addAutoShaderVar( _In_ ShaderBase *shader, const std::string &varName, ShaderAutoBindType varType )
	{
		// get auto variable list or create one if not already present
		std::pair< std::map< ShaderBase*, std::vector<AutoShaderVar> >::iterator, bool > shaderVars;
		shaderVars = m_constVarLinks.insert( std::pair< ShaderBase*, std::vector<AutoShaderVar> >( shader, std::vector<AutoShaderVar>() ) );

		// add shader var to list
		shaderVars.first->second.push_back( AutoShaderVar(varName, varType) );
	}


	void ShaderProgram::activate( _In_ Camera *renderCam, _In_ RenderableObject *renderObj )
	{
		std::map< ShaderBase*, std::vector<AutoShaderVar> >::iterator shaderIt;
		std::vector<AutoShaderVar>::iterator varIt;

		// bind shaders to pipeline
		if ( m_vertShader )
		{
			shaderIt = m_constVarLinks.find(m_vertShader);
			if ( shaderIt != m_constVarLinks.end() )
				for ( varIt = shaderIt->second.begin(); varIt != shaderIt->second.end(); ++varIt )
					updateAutoShaderVar( renderCam, renderObj, m_vertShader, *varIt );

			m_vertShader->activate();
		}
		if ( m_pixShader )
		{
			shaderIt = m_constVarLinks.find(m_pixShader);
			if ( shaderIt != m_constVarLinks.end() )
				for ( varIt = shaderIt->second.begin(); varIt != shaderIt->second.end(); ++varIt )
					updateAutoShaderVar( renderCam, renderObj, m_pixShader, *varIt );

			m_pixShader->activate();
		}
	}


	void ShaderProgram::updateAutoShaderVar( _In_ Camera *renderCam, _In_ RenderableObject *renderObj,
										_In_ ShaderBase *shader, AutoShaderVar shaderVar )
	{
		float tempFloat = 0.0f;
		DirectX::XMFLOAT4X4 tempFloat4x4;
		DirectX::XMFLOAT3 tempFloat3;
		switch ( shaderVar.type )
		{
			case ShaderAutoBindType::CameraProjectionMatrix:
				tempFloat4x4 = renderCam->getProjMatrix();
				shader->updateConstantVariable( shaderVar.name, &tempFloat4x4.m[0] );
				break;
			case ShaderAutoBindType::CameraViewMatrix:
				tempFloat4x4 = renderCam->getViewMatrix();
				shader->updateConstantVariable( shaderVar.name, &tempFloat4x4.m[0] );
				break;
			case ShaderAutoBindType::CameraPosition:
				tempFloat3 = renderCam->getPosition();
				shader->updateConstantVariable( shaderVar.name, &tempFloat );
				break;
			case ShaderAutoBindType::CameraTarget:
				tempFloat3 = renderCam->getTarget();
				shader->updateConstantVariable( shaderVar.name, &tempFloat );
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
			case ShaderAutoBindType::ObjectModelMatrix:
				tempFloat4x4 = renderObj->getModelMatrix();
				shader->updateConstantVariable( shaderVar.name, &tempFloat4x4.m[0] );
				break;
			case ShaderAutoBindType::ObjectNormalMatrix:
				tempFloat4x4 = renderObj->getNormalMatrix();
				shader->updateConstantVariable( shaderVar.name, &tempFloat4x4.m[0] );
				break;
			default:
				std::cout << "(ShaderProgram::updateAutoShaderVar) Warning: Unknown shader variable type:" << shaderVar.type << " No update done." << std::endl;
				break;
		}
	}
}
