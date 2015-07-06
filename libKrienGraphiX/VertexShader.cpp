
#include "ConstantBuffer.h"
#include "VertexShader.h"

namespace kgx
{
	VertexShader::VertexShader( _In_ ID3D11Device *dxDev, const VertexInputLayout &layout )
		: ShaderBase( dxDev ), m_shaderSource( nullptr ), m_vertShader( nullptr ), m_inputLayout( layout )
	{
	}


	VertexShader::~VertexShader()
	{
		if ( m_vertShader )
			m_vertShader->Release();
		if ( m_shaderSource )
			m_shaderSource->Release();
	}


	void VertexShader::activate()
	{
		m_dxDevCont->VSSetShader( m_vertShader, 0, 0 );
		m_dxDevCont->IASetInputLayout(m_inputLayout.getDxInputLayout(m_dxDev, m_shaderSource));

		UINT buffCounter = 0U;
		std::vector<ConstantBuffer*>::iterator it;
		for ( it = m_constBuffers.begin(); it != m_constBuffers.end(); ++it, ++buffCounter )
		{
			//TODO: iets verzinnen zodat alle buffers in 1x aangezet kunnen worden..
			(*it)->commit();
			ID3D11Buffer *buff = (*it)->getDxBufferPtr();
			m_dxDevCont->VSSetConstantBuffers(buffCounter, 1U, &buff);
		}
	}


	HRESULT VertexShader::build( _In_ ID3DBlob *source )
	{
		m_shaderSource = source;
		m_shaderSource->AddRef();
		return m_dxDev->CreateVertexShader( m_shaderSource->GetBufferPointer(), m_shaderSource->GetBufferSize(), NULL, &m_vertShader );
	}
}