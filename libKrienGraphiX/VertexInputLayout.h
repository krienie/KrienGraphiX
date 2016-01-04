
#pragma once

#include <d3d11.h>
#include <vector>

namespace kgx
{
	class VertexInputLayout
	{
		public:
			enum Type
			{
				Position,
				TextureCoordinate,
				Normal,
				Tangent
			};

			explicit VertexInputLayout( const std::vector<Type> &inputTypes = std::vector<Type>() );
			explicit VertexInputLayout( const VertexInputLayout &other );
			~VertexInputLayout();

			VertexInputLayout& operator=( const VertexInputLayout &other );

			void addInputType( Type t );
			void addInputType( const D3D11_INPUT_ELEMENT_DESC &inputDesc );
			std::vector<Type> getInputTypes() const;
			ID3D11InputLayout* getDxInputLayout( ID3D11Device *dxDevice, ID3DBlob *shaderSource, bool forceRebuild = false );
			UINT getBufferStride() const;


		private:
			std::vector<Type> m_inputTypes;
			std::vector<D3D11_INPUT_ELEMENT_DESC> m_inputDescriptor;
			ID3D11InputLayout *m_dxLayout;
			UINT m_bufferStride;

	};
}

