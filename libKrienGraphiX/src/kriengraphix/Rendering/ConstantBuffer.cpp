
#include "Rendering/ConstantBuffer.h"

#include <iostream>

namespace kgx
{
    ConstantBuffer::ConstantBuffer( ID3D11Device *m_dxDevice, UINT registerIndex, std::string name )
        : m_dxDev( m_dxDevice ), m_dxDevCont( nullptr ), m_dxBuffer( nullptr ), m_bufferSize( 0U ),
        m_registerIndex( registerIndex ), m_bufferName( name )
    {
        m_dxDev->GetImmediateContext( &m_dxDevCont );
    }

    ConstantBuffer::~ConstantBuffer()
    {
        if ( m_dxBuffer )
            m_dxBuffer->Release();
        if ( m_dxDevCont )
            m_dxDevCont->Release();
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

        m_bufferSize = sizeInBytes;

        // create buffer descriptor
        D3D11_BUFFER_DESC buffDesc;
        ZeroMemory( &buffDesc, sizeof( buffDesc ) );
        buffDesc.ByteWidth      = ((sizeInBytes - 1u) / 16u + 1u) * 16u;		//ByteWidth has to be a multiple of 16 for constant buffers;
        buffDesc.Usage          = D3D11_USAGE_DYNAMIC;
        buffDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT res = m_dxDev->CreateBuffer( &buffDesc, nullptr, &m_dxBuffer );
        if ( FAILED( res ) )
        {
            std::cout << "Error (ConstantBuffer::create): Error creating buffer" << std::endl;
            m_dxBuffer = nullptr;
        }

        return res;
    }

    void ConstantBuffer::copyBufferData( const void *data, UINT size )
    {
        copyBufferData( data, size, m_dxDevCont );
    }
    void ConstantBuffer::copyBufferData( const void *data, UINT size, ID3D11DeviceContext *dxDevCont )
    {
        D3D11_MAPPED_SUBRESOURCE mapRes;
        dxDevCont->Map( m_dxBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes );
        memcpy( mapRes.pData, data, min( size, m_bufferSize ) );
        dxDevCont->Unmap( m_dxBuffer, NULL );
    }
}
