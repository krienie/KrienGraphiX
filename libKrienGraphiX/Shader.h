
#pragma once

#include <d3d11.h>
#include <string>
#include <vector>

#include "ConstantBuffer.h"

namespace kgx
{
	class Shader
	{
		public:
			/*enum BufferType
			{
				CameraBuffer,
				ObjectBuffer,
				NumBufferTypes
			};*/

			bool load( const std::wstring &filename );

			void updateConstantVariable( const std::string &name, _In_ void *dataPtr );
			virtual void activate() = 0;

		protected:
			Shader( _In_ ID3D11Device *dxDevice );
			virtual ~Shader();

			virtual HRESULT build( _In_ ID3DBlob *shaderSource ) = 0;

			ID3D11Device *dxDev;
			ID3D11DeviceContext *dxDevCont;

			std::vector<ConstantBuffer*> constBuffers;

		private:
			// no copying allowed
			Shader( const Shader& );
			Shader& operator=( const Shader& );
	};
}
