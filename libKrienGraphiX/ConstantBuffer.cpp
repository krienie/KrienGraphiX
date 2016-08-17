
#include <iostream>

#include "ConstantBuffer.h"

namespace kgx
{
	ConstantBuffer::ConstantBuffer( ID3D11Device *m_dxDevice, UINT registerIndex, std::string name )
		: m_dxDev(m_dxDevice), m_dxDevCont(nullptr), m_dxBuffer(nullptr), m_bufferSize(0U),
			m_registerIndex(registerIndex), m_bufferName(name), m_dataChanged(false), m_variables(), m_rawData(nullptr)
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );
	}

	//TODO: create constructor that defines a ConstantBuffer based on a shader reflection struct, so you won't have to call addVariableDefinition() afterwards

	ConstantBuffer::ConstantBuffer( const ConstantBuffer &other )
		: m_dxDev(other.m_dxDev), m_dxDevCont(other.m_dxDevCont), m_dxBuffer(other.m_dxBuffer),
		m_bufferSize(other.m_bufferSize), m_dataChanged(other.m_dataChanged),
		m_variables(other.m_variables)
	{
		if ( m_dxBuffer )
			m_dxBuffer->AddRef();		//TODO: this might give some problems later on, maybe its better to just recreate the whole damn buffer
		if ( m_dxDevCont )
			m_dxDevCont->AddRef();		// same for the device context
		if ( other.m_rawData )
		{
			m_rawData = new UCHAR[m_bufferSize];
			memcpy( m_rawData, other.m_rawData, sizeof(UCHAR) * m_bufferSize );
		}
	}

	ConstantBuffer::~ConstantBuffer()
	{
		if ( m_rawData )
			delete[] m_rawData;
		if ( m_dxBuffer )
			m_dxBuffer->Release();
		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}

	ConstantBuffer& ConstantBuffer::operator= ( const ConstantBuffer &rhs )
	{
		if ( this != &rhs )
		{
			m_dxDev       = rhs.m_dxDev;
			m_dxDevCont   = rhs.m_dxDevCont;
			m_dxBuffer    = rhs.m_dxBuffer;
			m_bufferSize  = rhs.m_bufferSize;
			m_dataChanged = rhs.m_dataChanged;
			m_variables   = rhs.m_variables;

			if ( m_dxBuffer )
				m_dxBuffer->AddRef();
			if ( m_dxDevCont )
				m_dxDevCont->AddRef();
			if ( rhs.m_rawData )
			{
				m_rawData = new UCHAR[m_bufferSize];
				memcpy( m_rawData, rhs.m_rawData, sizeof( UCHAR ) * m_bufferSize );
			}
		}

		return *this;
	}


	UINT ConstantBuffer::getRegisterIndex() const
	{
		return m_registerIndex;
	}

	std::string ConstantBuffer::getName() const
	{
		return m_bufferName;
	}

	UINT ConstantBuffer::getBufferSize() const
	{
		return m_bufferSize;
	}

	ID3D11Buffer* ConstantBuffer::getDxBufferPtr() const
	{
		return m_dxBuffer;
	}


	HRESULT ConstantBuffer::create( UINT sizeInBytes )
	{
		// release resources if a buffer has already been created
		if ( m_dxBuffer )
		{
			m_dxBuffer->Release();
			m_dxBuffer = nullptr;
		}
		if ( m_rawData )
		{
			delete[] m_rawData;
			m_rawData = nullptr;
		}

		m_bufferSize = sizeInBytes;
		m_rawData    = new UCHAR[sizeInBytes];
		memset( m_rawData, 0, sizeInBytes );

		// create buffer descriptor
		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory( &buffDesc, sizeof(buffDesc) );
		buffDesc.ByteWidth      = ((sizeInBytes - 1u) / 16u + 1u) * 16u;		//ByteWidth has to be a multiple of 16 for constant buffers;
		buffDesc.Usage          = D3D11_USAGE_DYNAMIC;
		buffDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		// create initial data descriptor
		D3D11_SUBRESOURCE_DATA initDataDesc;
		ZeroMemory( &initDataDesc, sizeof( initDataDesc ) );
		initDataDesc.pSysMem = m_rawData;

		HRESULT res = m_dxDev->CreateBuffer( &buffDesc, &initDataDesc, &m_dxBuffer );
		if ( FAILED(res) )
		{
			std::cout << "Error (ConstantBuffer::create): Error creating buffer" << std::endl;
			m_dxBuffer = nullptr;
		}

		return res;
	}

	void ConstantBuffer::copyBufferData( void *data, UINT size )
	{
		D3D11_MAPPED_SUBRESOURCE mapRes;
		m_dxDevCont->Map(m_dxBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
		memcpy( mapRes.pData, data, min(size, m_bufferSize) );
		m_dxDevCont->Unmap(m_dxBuffer, NULL);
	}

	void ConstantBuffer::commit()
	{
		if ( !m_dataChanged || !m_rawData )
			return;

		//TODO: create mechanism so that only the changed data is committed, not the whole buffer => maybe different update types: one that commits immediately, and one that does not...
		D3D11_MAPPED_SUBRESOURCE mapRes;
		m_dxDevCont->Map(m_dxBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
		memcpy( mapRes.pData, m_rawData, m_bufferSize );
		m_dxDevCont->Unmap(m_dxBuffer, NULL);

		m_dataChanged = false;
	}


	bool ConstantBuffer::hasVariable( const std::string &name ) const
	{
		return m_variables.find(name) != m_variables.end();
	}

	void ConstantBuffer::addVariableDefinition( const std::string &name, UINT offset, UINT size )
	{
		m_variables.insert( std::pair<std::string, VarPosition>(name, VarPosition(offset,size)) );
	}


	/**
	 * Searches for a variable with the given name and updates its data when found.
	 * Returns true if the variable was found, false otherwise
	 */
	bool ConstantBuffer::updateVariable( const std::string &name, const void *data )
	{
		std::map< std::string, VarPosition >::iterator it = m_variables.find(name);

		if ( it != m_variables.end() )
		{
			memcpy( m_rawData + it->second.offset, data, it->second.size );
			m_dataChanged = true;
			return true;
		}

		return false;
	}
}
