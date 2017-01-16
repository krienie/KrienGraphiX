
#pragma once

#include <d3d11.h>
#include <vector>

#include "RenderableObject.h"

namespace kgx
{
	class Camera;

	class RenderPass
	{
		public:
			RenderPass( ID3D11DeviceContext *defDevCont, ID3D11RenderTargetView *rtv, ID3D11DepthStencilView *dsv,
							const DirectX::XMFLOAT4X4 &viewMatrix, const DirectX::XMFLOAT4X4 &projMatrix,
							ShaderProgram::ShaderProgramID shaderProgID );
			~RenderPass();

			void record( const std::vector<RenderableObject> &renderObjects, const LightData &lightData );
			void submit();

		private:
			ID3D11DeviceContext *m_dxDevCont;
			ID3D11DeviceContext *m_dxDeferredDevCont;
			ID3D11CommandList *m_commandList;
			ID3D11RenderTargetView *m_renderTargetView;
			ID3D11DepthStencilView *m_depthStencilView;
			DirectX::XMFLOAT4X4 m_viewMatrix;
			DirectX::XMFLOAT4X4 m_projMatrix;
			ShaderProgram::ShaderProgramID m_shaderProgID;
	};
}
