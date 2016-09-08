
/**
 * Inspired by Stefan Reinalter's blog on stateless, layered, multi-threaded rendering.
 * https://blog.molecular-matters.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/
 */

#pragma once

#include <d3d11.h>

namespace kgx
{
	typedef void(*BackendDispatchFunction)( const void* );

	namespace renderbackend
	{
		void bindMeshToIAStage( const void *data );
		void drawIndexed( const void *data );
		void copyConstantBufferData( const void *data );
		void updateConstantBufferVariable( const void *data );

		void setVertexShaderSamplers( const void *data );
		void setVertexShaderResourceViews( const void *data );

		void setPixelShaderSamplers( const void *data );
		void setPixelShaderResourceViews( const void *data );

		void resetState();

		void setDeviceContext( ID3D11Device *dxDevice );
		void releaseDeviceContext();
	}
}
