
#include "ConstantBuffer.h"
#include "TextureManager.h"
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

		// update all buffers if necessary
		std::vector<ConstantBuffer*>::iterator it;
		for ( it = m_constBuffers.begin(); it != m_constBuffers.end(); ++it )
			(*it)->commit();

		// set buffers
		m_dxDevCont->VSSetConstantBuffers( 0u, static_cast<UINT>(m_dxConstBuffers.size()), m_dxConstBuffers.data() );

		// set textures, if available
		if ( m_textures.size() > 0u )
		{
			ID3D11SamplerState *sampler = TextureManager::getInst()->getDefaultSampler();
			m_dxDevCont->VSSetSamplers( 0, 1, &sampler );
			m_dxDevCont->VSSetShaderResources( 0, UINT( m_texViews.size() ), m_texViews.data() );
		}
	}


	HRESULT VertexShader::build( _In_ ID3DBlob *source )
	{
		m_shaderSource = source;
		m_shaderSource->AddRef();
		return m_dxDev->CreateVertexShader( m_shaderSource->GetBufferPointer(), m_shaderSource->GetBufferSize(), NULL, &m_vertShader );
	}
}