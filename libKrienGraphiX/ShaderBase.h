
#pragma once

#include <d3d11.h>
#include <string>
#include <vector>

namespace kgx
{
	class ConstantBuffer;
	class Texture;

	class ShaderBase
	{
		public:
			ConstantBuffer* getConstantBufferPtrByIndex( UINT registerIndex ) const;

			HRESULT loadFromFile( const std::string &filename );
			HRESULT loadFromSource( const std::string &source, const std::string &entryPoint = "main" );

			void updateConstantVariable( const std::string &name, const void *dataPtr );

			virtual void commitChanges();
			virtual void activate() = 0;

		protected:
			ShaderBase( ID3D11Device *dxDevice, const std::string &target );
			virtual ~ShaderBase();

			virtual HRESULT build( ID3DBlob *shaderSource ) = 0;

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			std::vector<ConstantBuffer*> m_constBuffers;
			std::vector<ID3D11Buffer*> m_dxConstBuffers;		// used for setting all the constant buffers in one API call

			std::string m_target;

		private:
			// no copying allowed
			ShaderBase( const ShaderBase& );
			ShaderBase& operator=(const ShaderBase&);

			HRESULT processLoadedShaderBlob( ID3DBlob *shaderSource );
	};
}
