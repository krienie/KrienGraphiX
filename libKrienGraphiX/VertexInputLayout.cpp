
#include "VertexInputLayout.h"

namespace kgx
{
	VertexInputLayout::VertexInputLayout()
		: inputDescriptor(), dxLayout(nullptr), bufferStride(0U)
	{
	}

	VertexInputLayout::VertexInputLayout( const std::vector<Type> &inputTypes )
		: inputDescriptor(), dxLayout(nullptr), bufferStride(0U)
	{
		std::vector<Type>::const_iterator it;
		for ( it = inputTypes.begin(); it != inputTypes.end(); ++it )
			addInputType(*it);
	}

	VertexInputLayout::VertexInputLayout( const VertexInputLayout &other )
		: inputDescriptor(other.inputDescriptor), dxLayout(other.dxLayout), bufferStride(other.bufferStride)
	{
		if ( dxLayout )
			dxLayout->AddRef();
	}

	VertexInputLayout::~VertexInputLayout()
	{
		if ( dxLayout )
			dxLayout->Release();
	}


	VertexInputLayout& VertexInputLayout::operator=( const VertexInputLayout &other )
	{
		this->inputDescriptor = other.inputDescriptor;
		this->dxLayout        = other.dxLayout;
		this->bufferStride    = other.bufferStride;

		if ( this->dxLayout )
			this->dxLayout->AddRef();

		return *this;
	}


	void VertexInputLayout::addInputType( Type t )
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		ZeroMemory( &desc, sizeof(D3D11_INPUT_ELEMENT_DESC) );

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
				/* fall-through */
			default:
				desc.SemanticName = "TANGENT";
				break;
		}

		desc.SemanticIndex        = 0U;
		desc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
		desc.InputSlot            = 0U;
		desc.AlignedByteOffset    = sizeof(float) * 3U * static_cast<UINT>(inputDescriptor.size());
		desc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = 0U;

		inputDescriptor.push_back(desc);

		bufferStride += sizeof(float) * 3U;
	}


	ID3D11InputLayout* VertexInputLayout::getDxInputLayout( _In_ ID3D11Device *dxDevice, _In_ ID3DBlob *shaderSource, bool forceRebuild )
	{
		if ( dxLayout )
		{
			if ( !forceRebuild )
				return dxLayout;
			
			dxLayout->Release();
			dxLayout = nullptr;
		}

		HRESULT res = dxDevice->CreateInputLayout( inputDescriptor.data(), static_cast<UINT>(inputDescriptor.size()), shaderSource->GetBufferPointer(),
													shaderSource->GetBufferSize(), &dxLayout );

		if ( FAILED(res) )
			return nullptr;
		else return dxLayout;
	}


	UINT VertexInputLayout::getBufferStride() const
	{
		return bufferStride;
	}
}
