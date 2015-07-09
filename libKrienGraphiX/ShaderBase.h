
#pragma once

#include <d3d11.h>
#include <string>
#include <vector>

namespace kgx
{
	class ConstantBuffer;

	class ShaderBase
	{
		public:
			bool load( const std::wstring &filename );

			void updateConstantVariable( const std::string &name, _In_ void *dataPtr );
			virtual void activate() = 0;

		protected:
			explicit ShaderBase( _In_ ID3D11Device *dxDevice );
			virtual ~ShaderBase();

			virtual HRESULT build( _In_ ID3DBlob *shaderSource ) = 0;

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			std::vector<ConstantBuffer*> m_constBuffers;
			std::vector<ID3D11Buffer*> m_dxConstBuffers;		// used for setting all the constant buffers in one API call

		private:
			// no copying allowed
			ShaderBase( const ShaderBase& );
			ShaderBase& operator=(const ShaderBase&);
	};
}
