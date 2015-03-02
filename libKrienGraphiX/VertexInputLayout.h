
#pragma once

#include <d3d11.h>
#include <vector>

namespace kgx
{
	class VertexInputLayout
	{
		public:
			//TODO: add support for custom input types
			enum Type
			{
				Position,
				TextureCoordinate,
				Normal,
				Tangent
			};

			VertexInputLayout();
			VertexInputLayout( const std::vector<Type> &inputTypes );
			VertexInputLayout( const VertexInputLayout &other );
			~VertexInputLayout();

			VertexInputLayout& operator=( const VertexInputLayout &other );

			void addInputType( Type t );
			ID3D11InputLayout* getDxInputLayout( _In_ ID3D11Device *dxDevice, _In_ ID3DBlob *shaderSource, bool forceRebuild = false );
			UINT getBufferStride() const;


		private:
			std::vector<D3D11_INPUT_ELEMENT_DESC> inputDescriptor;
			ID3D11InputLayout *dxLayout;
			UINT bufferStride;

	};
}

