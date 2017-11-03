
#ifndef INITGUID
	#define INITGUID
#endif

#include "ShaderBase.h"

#include <iostream>
#include <d3d11shader.h>
#include <d3dcompiler.h>

#include "ConstantBuffer.h"
#include "Filesystem.h"


namespace kgx
{
	ShaderBase::ShaderBase( ID3D11Device *dxDevice, const std::string &target )
		: m_dxDev(dxDevice), m_dxDevCont(nullptr), m_constBuffers(), m_dxConstBuffers(),
			m_target(target)
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );
	}


	ShaderBase::~ShaderBase()
	{
		for ( auto &constBuffer : m_constBuffers )
			delete constBuffer;

		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}


	ConstantBuffer* ShaderBase::getConstantBufferPtrByIndex( UINT registerIndex ) const
	{
		if ( m_constBuffers.size() == 0u )
			return nullptr;

		// register indices are assumed to be ascending, starting from 0
		return m_constBuffers[registerIndex];
	}


	HRESULT ShaderBase::loadFromFile( const std::string &filename )
	{
		std::string shaderFile   = filesystem::getFile( filename );
		std::wstring wShaderFile = std::wstring( shaderFile.begin(), shaderFile.end() );

		//load shader file
		ID3DBlob *shaderSource;
		HRESULT res = D3DReadFileToBlob( wShaderFile.c_str(), &shaderSource );
		if ( FAILED(res) )
		{
			std::cout << "Error (ShaderBase::loadFromFile): Error loading shader source " << wShaderFile.c_str() << std::endl;
			return res;
		}

		res = processLoadedShaderBlob( shaderSource );
		if ( FAILED(res) )
		{
			std::cout << "Error (ShaderBase::loadFromFile): Error processing shader source " << wShaderFile.c_str() << std::endl;
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
		HRESULT res = D3DCompile( source.c_str(), sizeof( source ), nullptr, nullptr, nullptr, entryPoint.c_str(), m_target.c_str(), flags, 0u, &shaderSource, &compileErrors );
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

	void ShaderBase::activate()
	{
		activate( m_dxDevCont );
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

			//TODO: these can be put on the stack
			ConstantBuffer *cBuff = new ConstantBuffer( m_dxDev, i, shaderBuffDesc.Name );
			cBuff->create( shaderBuffDesc.Size );

			m_constBuffers.push_back( cBuff );
			m_dxConstBuffers.push_back( cBuff->getDxBufferPtr() );
		}

		reflector->Release();

		return S_OK;
	}
}
