
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


	void Material::addAutoShaderVar( _In_ Shader *shader, const std::string &varName, ShaderVarType varType )
	{
		// get auto variable list or create one if not already present
		std::pair< std::map< Shader*, std::vector<AutoShaderVar> >::iterator, bool > shaderVars; 
		shaderVars = m_constVarLinks.insert( std::pair< Shader*, std::vector<AutoShaderVar> >(shader, std::vector<AutoShaderVar>()) );

		// add shader var to list
		shaderVars.first->second.push_back( AutoShaderVar(varName, varType) );
	}


	void Material::activate( _In_ Camera *renderCam, _In_ RenderableObject *renderObj )
	{
		std::map< Shader*, std::vector<AutoShaderVar> >::iterator shaderIt;
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
											_In_ Shader *shader, AutoShaderVar shaderVar )
	{
		float tempFloat = 0.0f;
		DirectX::XMFLOAT4X4 tempFloat4x4;
		DirectX::XMFLOAT3 tempFloat3;
		switch ( shaderVar.type )
		{
			case ShaderVarType::CameraProjectionMatrix:
				tempFloat4x4 = renderCam->getProjMatrix();
				shader->updateConstantVariable( shaderVar.name, &tempFloat4x4.m[0] );
				break;
			case ShaderVarType::CameraViewMatrix:
				tempFloat4x4 = renderCam->getViewMatrix();
				shader->updateConstantVariable( shaderVar.name, &tempFloat4x4.m[0] );
				break;
			case ShaderVarType::CameraPosition:
				tempFloat3 = renderCam->getPosition();
				shader->updateConstantVariable( shaderVar.name, &tempFloat );
				break;
			case ShaderVarType::CameraTarget:
				tempFloat3 = renderCam->getTarget();
				shader->updateConstantVariable( shaderVar.name, &tempFloat );
				break;
			case ShaderVarType::CameraFieldOfView:
				tempFloat = renderCam->getFOV();
				shader->updateConstantVariable( shaderVar.name, &tempFloat );
				break;
			case ShaderVarType::CameraAspectRatio:
				tempFloat = renderCam->getAspectRatio();
				shader->updateConstantVariable( shaderVar.name, &tempFloat );
				break;
			case ShaderVarType::CameraNearZ:
				tempFloat = renderCam->getNearZ();
				shader->updateConstantVariable( shaderVar.name, &tempFloat );
				break;
			case ShaderVarType::CameraFarZ:
				tempFloat = renderCam->getFarZ();
				shader->updateConstantVariable( shaderVar.name, &tempFloat );
				break;
			case ShaderVarType::ObjectModelMatrix:
				tempFloat4x4 = renderObj->getModelMatrix();
				shader->updateConstantVariable( shaderVar.name, &tempFloat4x4.m[0] );
				break;
			default:
				std::cout << "Warning (Material::updateAutoShaderVar): Unknown shader variable type. No update done." << std::endl;
				break;
		}
	}
}
