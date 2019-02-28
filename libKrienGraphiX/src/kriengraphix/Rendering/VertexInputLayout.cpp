
#include "Rendering/VertexInputLayout.h"

namespace kgx
{
    VertexInputLayout::VertexInputLayout( const std::vector<Type> &inputTypes )
        : m_inputTypes(), m_inputDescriptor(), m_dxLayout( nullptr ), m_bufferStride( 0U )
    {
        for ( const auto &inputType : inputTypes )
            addInputType( inputType );
    }

    VertexInputLayout::VertexInputLayout( const VertexInputLayout &other )
        : m_inputTypes( other.m_inputTypes ), m_inputDescriptor( other.m_inputDescriptor ), m_dxLayout( other.m_dxLayout ), m_bufferStride( other.m_bufferStride )
    {
        if ( m_dxLayout )
            m_dxLayout->AddRef();
    }

    VertexInputLayout::~VertexInputLayout()
    {
        if ( m_dxLayout )
            m_dxLayout->Release();
    }


    VertexInputLayout& VertexInputLayout::operator=( const VertexInputLayout &other )
    {
        if ( this != &other )
        {
            this->m_inputTypes      = other.m_inputTypes;
            this->m_inputDescriptor = other.m_inputDescriptor;
            this->m_dxLayout        = other.m_dxLayout;
            this->m_bufferStride    = other.m_bufferStride;

            if ( this->m_dxLayout )
                this->m_dxLayout->AddRef();
        }

        return *this;
    }


    void VertexInputLayout::addInputType( Type t )
    {
        D3D11_INPUT_ELEMENT_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_INPUT_ELEMENT_DESC ) );

        switch ( t )
        {
            case Type::Position:
                desc.SemanticName = "POSITION";
                break;
            case Type::TextureCoordinate:
                desc.SemanticName = "TEXCOORD";
                break;
            case Type::Normal:
                desc.SemanticName = "NORMAL";
                break;
            case Type::Tangent:
                desc.SemanticName = "TANGENT";
            default:
                // there is no default semantic => exit function
                return;
        }

        desc.SemanticIndex        = 0U;
        desc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
        desc.InputSlot            = 0U;
        desc.AlignedByteOffset    = sizeof(float) * 3U * static_cast<UINT>(m_inputDescriptor.size());
        desc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
        desc.InstanceDataStepRate = 0U;

        m_inputTypes.push_back( t );
        addInputType( desc );
    }
    void VertexInputLayout::addInputType( const D3D11_INPUT_ELEMENT_DESC &inputDesc )
    {
        // this is quite a lazy way to support custom input-layouts, but ah well.. it's ok for the time being I guess..
        m_inputDescriptor.push_back( inputDesc );

        m_bufferStride += sizeof( float ) * 3U;

        //TODO: add custom input-layouts to m_inputTypes
    }

    std::vector<VertexInputLayout::Type> VertexInputLayout::getInputTypes() const
    {
        return m_inputTypes;
    }

    ID3D11InputLayout* VertexInputLayout::getDxInputLayout( ID3D11Device *dxDevice, ID3DBlob *shaderSource, bool forceRebuild )
    {
        if ( m_dxLayout )
        {
            if ( !forceRebuild )
                return m_dxLayout;

            if ( !shaderSource )
                return nullptr;

            m_dxLayout->Release();
            m_dxLayout = nullptr;
        }

        HRESULT res = dxDevice->CreateInputLayout( m_inputDescriptor.data(), static_cast<UINT>(m_inputDescriptor.size()), shaderSource->GetBufferPointer(),
                                                   shaderSource->GetBufferSize(), &m_dxLayout );

        if ( FAILED( res ) )
        {
            m_dxLayout = nullptr;
            return nullptr;
        }

        return m_dxLayout;
    }


    UINT VertexInputLayout::getBufferStride() const
    {
        return m_bufferStride;
    }
}
