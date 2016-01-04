
#pragma once

#include <DirectXMath.h>

#include "VertexInputLayout.h"
#include "ShaderBase.h"

namespace kgx
{
	class VertexShader : public ShaderBase
	{
		public:
			VertexShader( ID3D11Device *dxDev, const VertexInputLayout &layout );
			~VertexShader();

			void activate();
			VertexInputLayout* getInputLayout();

		protected:
			HRESULT build( ID3DBlob *shaderSource );

		private:
			// no copying allowed
			VertexShader( const VertexShader& );
			VertexShader& operator=( const VertexShader& );

			ID3DBlob *m_shaderSource;
			ID3D11VertexShader *m_vertShader;
			VertexInputLayout m_inputLayout;
	};
}
