
#include "VertexShader.h"

namespace kgx
{
	VertexShader::VertexShader( ID3D11Device *dxDev, const VertexInputLayout &layout )
		: ShaderBase(dxDev, "vs_5_0"), m_shaderSource(nullptr), m_vertShader(nullptr), m_inputLayout(layout)
	{
	}

	VertexShader::~VertexShader()
	{
		if ( m_vertShader )
			m_vertShader->Release();
		if ( m_shaderSource )
			m_shaderSource->Release();
	}

	void VertexShader::activate( ID3D11DeviceContext *dxContext )
	{
		dxContext->VSSetShader( m_vertShader, nullptr, 0 );
		dxContext->IASetInputLayout( m_inputLayout.getDxInputLayout( m_dxDev, m_shaderSource ) );

		dxContext->VSSetConstantBuffers( 0u, static_cast<UINT>(m_dxConstBuffers.size()), m_dxConstBuffers.data() );
	}

	VertexInputLayout* VertexShader::getInputLayout()
	{
		return &m_inputLayout;
	}

	HRESULT VertexShader::build( ID3DBlob *source )
	{
		m_shaderSource = source;
		m_shaderSource->AddRef();
		return m_dxDev->CreateVertexShader( m_shaderSource->GetBufferPointer(), m_shaderSource->GetBufferSize(), NULL, &m_vertShader );
	}
}