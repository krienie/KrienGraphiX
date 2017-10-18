
#pragma once

#include "VertexInputLayout.h"
#include "ShaderBase.h"

namespace kgx
{
	class VertexShader : public ShaderBase
	{
		public:
			VertexShader( ID3D11Device *dxDev, const VertexInputLayout &layout );
			~VertexShader();

			void activate( ID3D11DeviceContext *dxContext ) override;
			VertexInputLayout* getInputLayout();

		protected:
			HRESULT build( ID3DBlob *shaderSource ) override;

		private:
			// no copying allowed
			VertexShader( const VertexShader& ) = delete;
			VertexShader& operator=( const VertexShader& ) = delete;

			ID3DBlob *m_shaderSource;
			ID3D11VertexShader *m_vertShader;
			VertexInputLayout m_inputLayout;
	};
}
