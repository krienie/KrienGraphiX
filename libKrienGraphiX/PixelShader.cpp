
#include <iostream>

#include "ConstantBuffer.h"
#include "TextureManager.h"
#include "PixelShader.h"

namespace kgx
{
	PixelShader::PixelShader( ID3D11Device *dxDev )
		: ShaderBase( dxDev, "ps_5_0" ), m_pixelShader( 0 )//, structBuffers(), bufferUAVs()
	{
	}


	PixelShader::~PixelShader()
	{
		if (m_pixelShader)
			m_pixelShader->Release();

		/*std::vector<ID3D11Buffer*>::iterator buffIt;
		for ( buffIt = structBuffers.begin(); buffIt != structBuffers.end(); buffIt++ )
			(*buffIt)->Release();

		std::vector<ID3D11UnorderedAccessView*>::iterator uavIt;
		for ( uavIt = bufferUAVs.begin(); uavIt != bufferUAVs.end(); uavIt++ )
			(*uavIt)->Release();*/
	}


	void PixelShader::activate()
	{
		m_dxDevCont->PSSetShader(m_pixelShader, 0, 0);

		// update all buffers if necessary 
		std::vector<ConstantBuffer*>::iterator it;
		for ( it = m_constBuffers.begin(); it != m_constBuffers.end(); ++it )
			(*it)->commit();
		
		// set buffers
		m_dxDevCont->PSSetConstantBuffers(0u, static_cast<UINT>(m_dxConstBuffers.size()), m_dxConstBuffers.data());

		// set textures, if available
		if ( m_textures.size() > 0u )
		{
			ID3D11SamplerState *sampler = TextureManager::getInst()->getDefaultSampler();
			m_dxDevCont->PSSetSamplers( 0, 1, &sampler );
			m_dxDevCont->PSSetShaderResources( 0, UINT(m_texViews.size()), m_texViews.data() );
		}
	}


	HRESULT PixelShader::build( ID3DBlob *shaderSource )
	{
		return m_dxDev->CreatePixelShader(shaderSource->GetBufferPointer(), shaderSource->GetBufferSize(), NULL, &m_pixelShader);
	}


	/*ID3D11Buffer* PixelShader::createUAVBuffer( unsigned int structureStride, unsigned int numStructures, const void *buffData ) const
	{
		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory( &buffDesc, sizeof(buffDesc) );
		buffDesc.BindFlags           = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		buffDesc.ByteWidth           = numStructures * structureStride;
		buffDesc.StructureByteStride = structureStride;
		buffDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		buffDesc.Usage               = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA subResourceData;
		ZeroMemory( &subResourceData, sizeof( subResourceData ) );
		subResourceData.pSysMem = buffData;

		ID3D11Buffer *tempBuff;
		if ( FAILED(dxDev->CreateBuffer(&buffDesc, &subResourceData, &tempBuff)) )
		{
			std::cout << "PixelShader::createUAVBuffer aborted: Error creating UAV buffer." << std::endl;
			abort();
		}

		return tempBuff;
	}*/
}