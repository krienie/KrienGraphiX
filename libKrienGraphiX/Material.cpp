
#include <iostream>

#include "Camera.h"
#include "VertexInputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "RenderableObject.h"
#include "Material.h"

namespace kgx
{
	Material::Material( _In_ ID3D11Device *dxDevice )
		: m_dxDev(dxDevice), m_dxDevCont(nullptr), m_vertShader(nullptr),
			m_pixShader(nullptr), m_texViews(), m_texData(), m_sampler(nullptr), m_constVarLinks()
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );
	}

	Material::~Material()
	{
		if ( m_vertShader )
			delete m_vertShader;
		if ( m_pixShader )
			delete m_pixShader;
		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}


	VertexShader* Material::createVertexShader( const std::wstring &filename, const VertexInputLayout &layout )
	{
		m_vertShader = new VertexShader( m_dxDev, layout );

		if ( !m_vertShader->load(filename) )
		{
			// shader loading failed
			delete m_vertShader;
			m_vertShader = nullptr;
		}

		return m_vertShader;
	}

	VertexShader* Material::createVertexShader( const std::string &filename, const VertexInputLayout &layout )
	{
		std::wstring wFilename( filename.begin(), filename.end() );
		return createVertexShader( wFilename, layout );
	}

	PixelShader* Material::createPixelShader( const std::wstring &filename )
	{
		m_pixShader = new PixelShader( m_dxDev );

		if ( !m_pixShader->load(filename) )
		{
			// shader loading failed
			delete m_pixShader;
			m_pixShader = nullptr;
		}

		return m_pixShader;
	}

	PixelShader* Material::createPixelShader( const std::string &filename )
	{
		std::wstring wFilename( filename.begin(), filename.end() );
		return createPixelShader( wFilename );
	}


	VertexShader* Material::getVertexShader() const
	{
		return m_vertShader;
	}

	PixelShader* Material::getPixelShader() const
	{
		return m_pixShader;
	}


	void Material::addTexture( _In_ ID3D11ShaderResourceView* texView, _In_ ID3D11Resource* texture )
	{
		m_texViews.push_back(texView);
		m_texData.push_back(texture);
	}

	void Material::setSampler( _In_ ID3D11SamplerState *samp )
	{
		m_sampler = samp;
	}


	void Material::addAutoShaderVar( _In_ ShaderBase *shader, const std::string &varName, ShaderAutoBindType varType )
	{
		// get auto variable list or create one if not already present
		std::pair< std::map< ShaderBase*, std::vector<AutoShaderVar> >::iterator, bool > shaderVars;
		shaderVars = m_constVarLinks.insert( std::pair< ShaderBase*, std::vector<AutoShaderVar> >( shader, std::vector<AutoShaderVar>() ) );

		// add shader var to list
		shaderVars.first->second.push_back( AutoShaderVar(varName, varType) );
	}


	void Material::activate( _In_ Camera *renderCam, _In_ RenderableObject *renderObj )
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

		//TODO: activate buffers/textures/other resources
	}


	void Material::updateAutoShaderVar( _In_ Camera *renderCam, _In_ RenderableObject *renderObj,
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
			default:
				std::cout << "(Material::updateAutoShaderVar) Warning: Unknown shader variable type:" << shaderVar.type << " No update done." << std::endl;
				break;
		}
	}
}
