
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
			ConstantBuffer( ID3D11Device *dxDevice, UINT registerIndex, std::string bufferName );
			explicit ConstantBuffer( const ConstantBuffer &other );
			~ConstantBuffer();

			ConstantBuffer& operator=( const ConstantBuffer &rhs );

			UINT getRegisterIndex() const;
			std::string getName() const;
			UINT getBufferSize() const;
			ID3D11Buffer* getDxBufferPtr() const;


			HRESULT create( UINT sizeInBytes );
			void copyBufferData( void *data, UINT size );

			// methods below are deprecated
			void commit();
			bool hasVariable( const std::string &name ) const;
			void addVariableDefinition( const std::string &name, UINT offset, UINT size );
			/** Searches for a variable with the given name and updates its data when found. Returns true if the variable was found, false otherwise */
			bool updateVariable( const std::string &name, const void *data );
			// end deprecated

		private:
			struct VarPosition
			{
				VarPosition( UINT o, UINT s ) : offset(o), size(s) { }

				UINT offset;
				UINT size;
			};

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;
			ID3D11Buffer *m_dxBuffer;
			UINT m_bufferSize;

			UINT m_registerIndex;
			std::string m_bufferName;

			bool m_dataChanged;
			std::map<std::string, VarPosition> m_variables;
			UCHAR *m_rawData;

	};
}
