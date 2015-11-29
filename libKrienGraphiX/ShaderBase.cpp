
#ifndef INITGUID
	#define INITGUID
#endif

#include <iostream>
#include <d3d11shader.h>
#include <d3dcompiler.h>

#include "ConstantBuffer.h"
#include "Texture.h"
#include "ShaderBase.h"


namespace kgx
{
	ShaderBase::ShaderBase( ID3D11Device *dxDevice, const std::string &target )
		: m_dxDev(dxDevice), m_dxDevCont(nullptr), m_constBuffers(), m_dxConstBuffers(),
		m_textures(), m_texBuffers(), m_texViews(), m_target(target)
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );
	}


	ShaderBase::~ShaderBase()
	{
		std::vector<ConstantBuffer*>::iterator it;
		for ( it = m_constBuffers.begin(); it != m_constBuffers.end(); ++it )
			delete *it;

		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}


	HRESULT ShaderBase::loadFromFile( const std::wstring &filename )
	{
		//load shader file
		ID3DBlob *shaderSource;
		HRESULT res = D3DReadFileToBlob( filename.c_str(), &shaderSource );
		if ( FAILED(res) )
		{
			std::cout << "Error (ShaderBase::loadFromFile): Error loading shader source " << filename.c_str() << std::endl;
			return res;
		}

		res = processLoadedShaderBlob( shaderSource );
		if ( FAILED(res) )
		{
			std::cout << "Error (ShaderBase::loadFromFile): Error processing shader source " << filename.c_str() << std::endl;
			shaderSource->Release();
			return res;
		}
		
		shaderSource->Release();

		return S_OK;
	}

	HRESULT ShaderBase::loadFromSource( const std::string &source, const std::string &entryPoint )
	{
		//load shader source
		unsigned int flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL0;
#else
		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		ID3DBlob *shaderSource;
		ID3DBlob *compileErrors;
		HRESULT res = D3DCompile( source.c_str(), sizeof( source ), NULL, NULL, NULL, entryPoint.c_str(), m_target.c_str(), flags, 0u, &shaderSource, &compileErrors );
		if ( FAILED(res)  )
		{
			//TODO: parse compile errors
			std::cout << "Error (ShaderBase::loadFromSource): Error loading shader source " << std::endl;
			return res;
		}

		if ( FAILED( processLoadedShaderBlob( shaderSource ) ) )
		{
			std::cout << "Error (ShaderBase::loadFromSource): Error processing shader source " << std::endl;
			shaderSource->Release();
			return res;
		}

		shaderSource->Release();

		return S_OK;
	}


	void ShaderBase::updateConstantVariable( const std::string &name, const void *dataPtr )
	{
		//TODO: handle cases with duplicate variable names (two buffers, one variable in each buffer with the same name)

		if ( dataPtr )
		{
			std::vector<ConstantBuffer*>::iterator it;
			for ( it = m_constBuffers.begin(); it != m_constBuffers.end(); ++it )
				if ( (*it)->updateVariable( name, dataPtr ) )
					return;

			std::cout << "Warning (ShaderBase::updateConstantVariable): Variable with name " << name << " was not found. No update done." << std::endl;
		}
	}


	void ShaderBase::addTexture( Texture *tex )
	{
		if ( tex )
		{
			m_textures.push_back( tex );
			m_texBuffers.push_back( tex->getBuffer() );
			m_texViews.push_back( tex->getResourceView() );
		}
	}

	HRESULT ShaderBase::processLoadedShaderBlob( ID3DBlob *shaderSource )
	{
		HRESULT res = build( shaderSource );
		if ( FAILED(res) )
		{
			std::cout << "Error (ShaderBase::processLoadedShaderBlob): Error creating shader" << std::endl;
			return res;
		}


		// setup buffers using the shader reflection
		ID3D11ShaderReflection *reflector;
		res = D3DReflect( shaderSource->GetBufferPointer(), shaderSource->GetBufferSize(),
							IID_ID3D11ShaderReflection, (void**)&reflector );
		if ( FAILED( res ) )
		{
			std::cout << "Error (ShaderBase::processLoadedShaderBlob): Error creating shader reflection" << std::endl;
			return res;
		}


		D3D11_SHADER_DESC shaderDesc;
		ZeroMemory( &shaderDesc, sizeof( D3D11_SHADER_DESC ) );
		res = reflector->GetDesc( &shaderDesc );
		if ( FAILED( res ) )
		{
			std::cout << "Error (ShaderBase::processLoadedShaderBlob): Error creating shader reflection description" << std::endl;
			reflector->Release();
			return res;
		}


		for ( UINT i = 0U; i < shaderDesc.ConstantBuffers; ++i )
		{
			ID3D11ShaderReflectionConstantBuffer *constBuffReflection = reflector->GetConstantBufferByIndex( i );
			D3D11_SHADER_BUFFER_DESC shaderBuffDesc;
			constBuffReflection->GetDesc( &shaderBuffDesc );

			ConstantBuffer *cBuff = new ConstantBuffer( m_dxDev );
			cBuff->create( shaderBuffDesc.Size );
			for ( UINT j = 0U; j < shaderBuffDesc.Variables; ++j )
			{
				ID3D11ShaderReflectionVariable *variableRefl = constBuffReflection->GetVariableByIndex( j );
				D3D11_SHADER_VARIABLE_DESC shaderVarDesc;
				variableRefl->GetDesc( &shaderVarDesc );

				cBuff->addVariableDefinition( shaderVarDesc.Name, shaderVarDesc.StartOffset, shaderVarDesc.Size );
			}

			m_constBuffers.push_back( cBuff );
			m_dxConstBuffers.push_back( cBuff->getDxBufferPtr() );
		}

		reflector->Release();

		return S_OK;
	}
}
