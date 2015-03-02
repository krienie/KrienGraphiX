
#include <iostream>

#include "ConstantBuffer.h"

namespace kgx
{
	ConstantBuffer::ConstantBuffer( _In_ ID3D11Device *dxDevice )
		: dxDev(dxDevice), dxDevCont(nullptr), dxBuffer(nullptr),
			bufferElementSize(0U), dataChanged(false), variables(), rawData(nullptr)
	{
		dxDev->GetImmediateContext( &dxDevCont );
	}

	ConstantBuffer::~ConstantBuffer()
	{
		if ( rawData )
			delete[] rawData;
		if ( dxBuffer )
			dxBuffer->Release();
		if ( dxDevCont )
			dxDevCont->Release();
	}


	ID3D11Buffer* ConstantBuffer::getDxBuffer() const
	{
		return dxBuffer;
	}


	HRESULT ConstantBuffer::create( UINT sizeInBytes )
	{
		// release resources if a buffer has already been created
		if ( dxBuffer )
		{
			dxBuffer->Release();
			dxBuffer = nullptr;
		}
		if ( rawData )
		{
			delete[] rawData;
			rawData = nullptr;
		}
		//rawData.clear();

		// create buffer descriptor
		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory( &buffDesc, sizeof(buffDesc) );

		buffDesc.ByteWidth      = ((sizeInBytes - 1) / 16 + 1) * 16;		//ByteWidth has to be a multiple of 16 for constant buffers;
		buffDesc.Usage          = D3D11_USAGE_DYNAMIC;
		buffDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT res = dxDev->CreateBuffer( &buffDesc, NULL, &dxBuffer );
		if ( FAILED(res) )
		{
			std::cout << "Error (ConstantBuffer::create): Error creating buffer" << std::endl;
			dxBuffer = nullptr;
		}

		bufferElementSize = sizeInBytes;
		rawData = new UCHAR[sizeInBytes];
		//rawData.reserve(sizeInBytes);
		//std::cout << "capacity: " << rawData.capacity() << std::endl;

		return res;
	}




	void ConstantBuffer::commit()
	{
		if ( !dataChanged || !rawData )
			return;

		D3D11_MAPPED_SUBRESOURCE mapRes;
		dxDevCont->Map(dxBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
		memcpy( mapRes.pData, rawData, bufferElementSize );
		dxDevCont->Unmap(dxBuffer, NULL);

		dataChanged = false;
	}


	bool ConstantBuffer::hasVariable( const std::string &name ) const
	{
		return variables.find(name) != variables.end();
	}

	void ConstantBuffer::addVariableDefinition( const std::string &name, UINT offset, UINT size )
	{
		variables.insert( std::pair<std::string, VarPosition>(name, VarPosition(offset,size)) );
	}


	/**
	 * Searches for a variable with the given name and updates its data when found.
	 * Returns true if the variable was found, false otherwise
	 */
	bool ConstantBuffer::updateVariable( const std::string &name, const void *var )
	{
		std::map< std::string, VarPosition >::iterator it = variables.find(name);

		if ( it != variables.end() )
		{
			memcpy( rawData + it->second.offset, var, it->second.size );
			dataChanged = true;
			return true;
		} else
		{
			return false;
		}
	}
}
