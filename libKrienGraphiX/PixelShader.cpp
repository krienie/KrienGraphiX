
#include <iostream>

#include "ConstantBuffer.h"
#include "PixelShader.h"

namespace kgx
{
	PixelShader::PixelShader( _In_ ID3D11Device *dxDev )
		: ShaderBase( dxDev ), m_pixelShader( 0 )//, structBuffers(), bufferUAVs()
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

		UINT buffCounter = 0U;
		std::vector<ConstantBuffer*>::iterator it;
		for ( it = m_constBuffers.begin(); it != m_constBuffers.end(); ++it, ++buffCounter )
		{
			//TODO: iets verzinnen zodat alle buffers in 1x aangezet kunnen worden..
			(*it)->commit();
			ID3D11Buffer *buff = (*it)->getDxBufferPtr();
			m_dxDevCont->PSSetConstantBuffers(buffCounter, 1U, &buff);
		}
	}


	HRESULT PixelShader::build( _In_ ID3DBlob *shaderSource )
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