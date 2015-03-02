
#include <iostream>

#include "Camera.h"
#include "RenderableObject.h"
#include "Material.h"

namespace kgx
{
	Material::Material( _In_ ID3D11Device *dxDevice )
		: dxDev(dxDevice), dxDevCont(nullptr), vertShader(nullptr),
			pixShader(nullptr), texViews(), texData(), sampler(nullptr), constVarLinks()
	{
		dxDev->GetImmediateContext( &dxDevCont );
	}

	Material::~Material()
	{
		if ( vertShader )
			delete vertShader;
		if ( pixShader )
			delete pixShader;
		if ( dxDevCont )
			dxDevCont->Release();
	}


	VertexShader* Material::createVertexShader( const std::wstring &filename, const VertexInputLayout &layout )
	{
		vertShader = new VertexShader( dxDev, layout );

		if ( !vertShader->load(filename) )
		{
			// shader loading failed
			delete vertShader;
			vertShader = nullptr;
		}

		return vertShader;
	}

	PixelShader* Material::createPixelShader( const std::wstring &filename )
	{
		pixShader = new PixelShader( dxDev );

		if ( !pixShader->load(filename) )
		{
			// shader loading failed
			delete pixShader;
			pixShader = nullptr;
		}

		return pixShader;
	}

	VertexShader* Material::getVertexShader() const
	{
		return vertShader;
	}

	PixelShader* Material::getPixelShader() const
	{
		return pixShader;
	}


	void Material::addTexture( _In_ ID3D11ShaderResourceView* texView, _In_ ID3D11Resource* texture )
	{
		texViews.push_back(texView);
		texData.push_back(texture);
	}

	void Material::setSampler( _In_ ID3D11SamplerState *samp )
	{
		sampler = samp;
	}


	void Material::addAutoShaderVar( _In_ Shader *shader, const std::string &varName, ShaderVarType varType )
	{
		// get auto variable list or create one if not already present
		std::pair< std::map< Shader*, std::vector<AutoShaderVar> >::iterator, bool > shaderVars; 
		shaderVars = constVarLinks.insert( std::pair< Shader*, std::vector<AutoShaderVar> >(shader, std::vector<AutoShaderVar>()) );

		// add shader var to list
		shaderVars.first->second.push_back( AutoShaderVar(varName, varType) );
	}


	void Material::activate( _In_ Camera *renderCam, _In_ RenderableObject *renderObj )
	{
		std::map< Shader*, std::vector<AutoShaderVar> >::iterator shaderIt;
		std::vector<AutoShaderVar>::iterator varIt;

		// bind shaders to pipeline
		if ( vertShader )
		{
			shaderIt = constVarLinks.find(vertShader);
			if ( shaderIt != constVarLinks.end() )
				for ( varIt = shaderIt->second.begin(); varIt != shaderIt->second.end(); ++varIt )
					updateAutoShaderVar( renderCam, renderObj, vertShader, *varIt );

			vertShader->activate();
		}
		if ( pixShader )
		{
			shaderIt = constVarLinks.find(pixShader);
			if ( shaderIt != constVarLinks.end() )
				for ( varIt = shaderIt->second.begin(); varIt != shaderIt->second.end(); ++varIt )
					updateAutoShaderVar( renderCam, renderObj, pixShader, *varIt );

			pixShader->activate();
		}

		//TODO: activate buffers/textures/other resources
	}


	void Material::updateAutoShaderVar( _In_ Camera *renderCam, _In_ RenderableObject *renderObj,
											_In_ Shader *shader, AutoShaderVar shaderVar )
	{
		float tempFloat = 0.0f;
		DirectX::XMFLOAT4X4 tempFloat4x4;
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
				shader->updateConstantVariable( shaderVar.name, &renderCam->getPosition() );
				break;
			case ShaderVarType::CameraTarget:
				shader->updateConstantVariable( shaderVar.name, &renderCam->getTarget() );
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
