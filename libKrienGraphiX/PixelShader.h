
#pragma once

#include <vector>

#include "ShaderBase.h"

namespace kgx
{
	class PixelShader : public ShaderBase
	{
		public:
			explicit PixelShader( ID3D11Device *dxDev );
			~PixelShader();

			void activate( ID3D11DeviceContext *dxContext );

		protected:
			HRESULT build( ID3DBlob *shaderSource );

		private:
			// no copying allowed
			PixelShader( const PixelShader& );
			PixelShader& operator=( const PixelShader& );

			//ID3D11Buffer* createUAVBuffer( unsigned int structureStride, unsigned int numStructures, const void *buffData ) const;

			ID3D11PixelShader *m_pixelShader;

			//std::vector<ID3D11Buffer*> structBuffers;
			//std::vector<ID3D11UnorderedAccessView*> bufferUAVs;
	};
}