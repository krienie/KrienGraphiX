
#include "VertexShader.h"

namespace kgx
{
	VertexShader::VertexShader( _In_ ID3D11Device *dxDev, const VertexInputLayout &layout )
		: Shader( dxDev ), shaderSource(nullptr), vertShader(nullptr), inputLayout(layout)
	{
	}


	VertexShader::~VertexShader()
	{
		if ( vertShader )
			vertShader->Release();
		if ( shaderSource )
			shaderSource->Release();
	}


	void VertexShader::activate()
	{
		dxDevCont->VSSetShader( vertShader, 0, 0 );
		dxDevCont->IASetInputLayout( inputLayout.getDxInputLayout(dxDev, shaderSource) );

		UINT buffCounter = 0U;
		std::vector<ConstantBuffer*>::iterator it;
		for ( it = constBuffers.begin(); it != constBuffers.end(); ++it, ++buffCounter )
		{
			//TODO: iets verzinnen zodat alle buffers in 1x aangezet kunnen worden..
			(*it)->commit();
			ID3D11Buffer *buff = (*it)->getDxBuffer();
			dxDevCont->VSSetConstantBuffers( buffCounter, 1U, &buff );
		}
	}


	HRESULT VertexShader::build( _In_ ID3DBlob *source )
	{
		shaderSource = source;
		shaderSource->AddRef();
		return dxDev->CreateVertexShader( shaderSource->GetBufferPointer(), shaderSource->GetBufferSize(), NULL, &vertShader );
	}
}