
#pragma once

#include <d3d11.h>
#include <vector>

namespace kgx
{
	class Camera;
	class ShaderProgram;

	class RenderCore
	{
		public:
			RenderCore( ID3D11Device *dxDevice );
			~RenderCore();

			ID3D11DepthStencilView* getDepthStencilView() const;

			//TODO: add support for multiple render passes
			void setShaderProgram( ShaderProgram *shaderProg );

			bool init( UINT depthBuffWidth, UINT depthBuffHeight, UINT sampleCount, UINT sampleQuality );
			void renderFrame( Camera *camera );

		private:
			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;
			ID3D11DepthStencilView *m_depthStencilView;
			ID3D11RasterizerState *m_rasterizer;

			ShaderProgram *m_curShaderProg;
	};
}
