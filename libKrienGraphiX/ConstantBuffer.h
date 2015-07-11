
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <map>
#include <vector>


namespace kgx
{
	class ConstantBuffer
	{
		public:
			explicit ConstantBuffer( _In_ ID3D11Device *dxDevice );
			explicit ConstantBuffer( const ConstantBuffer &other );
			~ConstantBuffer();

			ConstantBuffer& operator=( const ConstantBuffer &rhs );


			ID3D11Buffer* getDxBufferPtr() const;
			HRESULT create( UINT sizeInBytes );
			void commit();
			bool hasVariable( const std::string &name ) const;
			void addVariableDefinition( const std::string &name, UINT offset, UINT size );
			/** Searches for a variable with the given name and updates its data when found. Returns true if the variable was found, false otherwise */
			bool updateVariable( const std::string &name, const void *var );

		private:

			struct VarPosition
			{
				VarPosition( UINT o, UINT s ) : offset(o), size(s) { }

				UINT offset;
				UINT size;

				bool operator==( const VarPosition &rhs ) const
				{
					return offset == rhs.offset && size == rhs.size;
				}
			};

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;
			ID3D11Buffer *m_dxBuffer;
			UINT m_bufferElementSize;

			bool m_dataChanged;
			std::map<std::string, VarPosition> m_variables;
			UCHAR *m_rawData;

	};
}
