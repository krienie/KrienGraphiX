
#include "Rendering/MeshBuffer.h"

#include <comdef.h>
#include <iostream>

#include "Core/KGXCore.h"

namespace kgx
{
    MeshBuffer::MeshBuffer( const std::vector<float> &vertices, const std::vector<UINT> &indices,
                            UINT vertexBufferStride )
        : m_dxDev( nullptr ), m_isInit( false ), m_vertexBufferStride( vertexBufferStride ),
        m_vertBuff( nullptr ), m_indexBuff( nullptr )
    {
        if ( vertices.size() <= 0 || indices.size() <= 0 )
        {
            std::cout << "Error (MeshBuffer::MeshBuffer): Vertices or indices not specified => vector is empty" << std::endl;
            return;
        }

        m_dxDev = KGXCore::get()->getDxDevicePtr();

        D3D11_SUBRESOURCE_DATA vertResource;
        vertResource.pSysMem          = vertices.data();
        vertResource.SysMemPitch      = 0;
        vertResource.SysMemSlicePitch = 0;

        D3D11_BUFFER_DESC buffDesc;
        ZeroMemory( &buffDesc, sizeof( buffDesc ) );

        buffDesc.ByteWidth = sizeof( float ) * UINT( vertices.size() );
        buffDesc.Usage     = D3D11_USAGE_IMMUTABLE;
        buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        HRESULT result = m_dxDev->CreateBuffer( &buffDesc, &vertResource, &m_vertBuff );
        if ( FAILED( result ) )
        {
            _com_error error( result );
            std::cout << "Error (MeshBuffer::MeshBuffer): " << error.ErrorMessage() << std::endl;
            return;
        }

        buffDesc.ByteWidth = sizeof( UINT ) * UINT( indices.size() );
        buffDesc.Usage     = D3D11_USAGE_IMMUTABLE;
        buffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA indexResource;
        indexResource.pSysMem          = indices.data();
        indexResource.SysMemPitch      = 0;
        indexResource.SysMemSlicePitch = 0;

        result = m_dxDev->CreateBuffer( &buffDesc, &indexResource, &m_indexBuff );
        if ( FAILED( result ) )
        {
            m_vertBuff->Release();
            m_vertBuff = nullptr;

            _com_error error( result );
            std::cout << "Error (MeshBuffer::MeshBuffer): " << error.ErrorMessage() << std::endl;
            return;
        }

        m_isInit = true;
    }

    MeshBuffer::~MeshBuffer()
    {
        if ( m_vertBuff )
            m_vertBuff->Release();
        if ( m_indexBuff )
            m_indexBuff->Release();
    }

    bool MeshBuffer::isInit() const
    {
        return m_isInit;
    }

    ID3D11Buffer* MeshBuffer::getVertexBuffer() const
    {
        return m_vertBuff;
    }

    ID3D11Buffer* MeshBuffer::getIndexBuffer() const
    {
        return m_indexBuff;
    }

    UINT MeshBuffer::getVertexBufferStride() const
    {
        return m_vertexBufferStride;
    }
}
