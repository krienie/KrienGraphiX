
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
			~ConstantBuffer();

			

			UINT getRegisterIndex() const;
			std::string getName() const;
			UINT getBufferSize() const;
			ID3D11Buffer* getDxBufferPtr() const;


			HRESULT create( UINT sizeInBytes );
			void copyBufferData( const void *data, UINT size );
			void copyBufferData( const void *data, UINT size, ID3D11DeviceContext *dxDevCont );

		private:
			// no copying allowed
			ConstantBuffer( const ConstantBuffer &other );
			ConstantBuffer& operator=( const ConstantBuffer &rhs );

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;
			ID3D11Buffer *m_dxBuffer;
			UINT m_bufferSize;

			UINT m_registerIndex;
			std::string m_bufferName;
	};
}
