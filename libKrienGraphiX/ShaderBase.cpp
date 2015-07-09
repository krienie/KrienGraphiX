
#ifndef INITGUID
	#define INITGUID
#endif

#include <iostream>
#include <d3d11shader.h>
#include <d3dcompiler.h>

#include "ConstantBuffer.h"
#include "ShaderBase.h"


namespace kgx
{
	ShaderBase::ShaderBase( _In_ ID3D11Device *dxDevice )
		: m_dxDev(dxDevice), m_dxDevCont(nullptr), m_constBuffers(), m_dxConstBuffers()
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


	bool ShaderBase::load( const std::wstring &filename )
	{
		//load shader source
		ID3DBlob *shaderSource;
		if ( FAILED(D3DReadFileToBlob( filename.c_str(), &shaderSource )) )
		{
			std::cout << "Error (Shader::load): Error loading shader source" << std::endl;
			return false;
		}


		HRESULT res = build( shaderSource );
		if ( FAILED(res) )
		{
			std::cout << "Error (Shader::load): Error creating shader" << std::endl;
			return false;
		}


		// setup buffers using the shader reflection
		ID3D11ShaderReflection *reflector;
		D3DReflect( shaderSource->GetBufferPointer(), shaderSource->GetBufferSize(), 
						IID_ID3D11ShaderReflection, (void**) &reflector);

		D3D11_SHADER_DESC shaderDesc;
		ZeroMemory( &shaderDesc, sizeof(D3D11_SHADER_DESC) );
		reflector->GetDesc( &shaderDesc );

		for ( UINT i = 0U; i < shaderDesc.ConstantBuffers; ++i )
		{
			ID3D11ShaderReflectionConstantBuffer *constBuffReflection = reflector->GetConstantBufferByIndex(i);
			D3D11_SHADER_BUFFER_DESC shaderBuffDesc;
			constBuffReflection->GetDesc( &shaderBuffDesc );

			ConstantBuffer *cBuff = new ConstantBuffer(m_dxDev);
			cBuff->create( shaderBuffDesc.Size );
			for ( UINT j = 0U; j < shaderBuffDesc.Variables; ++j )
			{
				ID3D11ShaderReflectionVariable *variableRefl = constBuffReflection->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC shaderVarDesc;
				variableRefl->GetDesc( &shaderVarDesc );

				cBuff->addVariableDefinition( shaderVarDesc.Name, shaderVarDesc.StartOffset, shaderVarDesc.Size );
			}

			m_constBuffers.push_back( cBuff );
			m_dxConstBuffers.push_back( cBuff->getDxBufferPtr() );
		}

		reflector->Release();
		shaderSource->Release();

		return true;
	}


	void ShaderBase::updateConstantVariable( const std::string &name, _In_ void *dataPtr )
	{
		std::vector<ConstantBuffer*>::iterator it;
		for ( it = m_constBuffers.begin(); it != m_constBuffers.end(); ++it )
		{
			if ( (*it)->updateVariable(name, dataPtr) )
				return;
		}

		std::cout << "Warning (ShaderBase::updateConstantVariable): Variable with name " << name << " was not found. No update done." << std::endl;
	}
}