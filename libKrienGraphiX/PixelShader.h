
#pragma once

#include "ShaderBase.h"

namespace kgx
{
	class PixelShader : public ShaderBase
	{
		public:
			explicit PixelShader( ID3D11Device *dxDev );
			~PixelShader();

			void activate( ID3D11DeviceContext *dxContext ) override;

		protected:
			HRESULT build( ID3DBlob *shaderSource ) override;

		private:
			// no copying allowed
			PixelShader( const PixelShader& )            = delete;
			PixelShader& operator=( const PixelShader& ) = delete;

			//ID3D11Buffer* createUAVBuffer( unsigned int structureStride, unsigned int numStructures, const void *buffData ) const;

			ID3D11PixelShader *m_pixelShader;

			//std::vector<ID3D11Buffer*> structBuffers;
			//std::vector<ID3D11UnorderedAccessView*> bufferUAVs;
	};
}