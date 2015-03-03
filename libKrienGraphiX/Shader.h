
#pragma once

#include <d3d11.h>
#include <string>
#include <vector>

//#include "ConstantBuffer.h"

namespace kgx
{
	class ConstantBuffer;

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
			explicit Shader( _In_ ID3D11Device *dxDevice );
			virtual ~Shader();

			virtual HRESULT build( _In_ ID3DBlob *shaderSource ) = 0;

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			std::vector<ConstantBuffer*> m_constBuffers;

		private:
			// no copying allowed
			Shader( const Shader& );
			Shader& operator=( const Shader& );
	};
}
