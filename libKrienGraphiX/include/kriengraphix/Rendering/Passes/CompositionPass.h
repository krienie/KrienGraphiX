
#pragma once

#include "Rendering/Passes/RenderPass.h"
#include "Rendering/ShaderProgramLibrary.h"

struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;

namespace kgx
{
	class ShaderProgram;

	class CompositionPass : public RenderPass
	{
		public:
			CompositionPass( ID3D11DeviceContext *deferredDevCont, ID3D11DepthStencilView *dsv, ID3D11RenderTargetView *rtv );

			virtual ~CompositionPass();

			void record( const std::vector<RenderableObject*> &renderObjects, const LightData &lightData,
						 const TextureLibrary &sceneTextures ) override;

		private:
			ShaderProgram *m_shaderProg;
			ShaderProgramLibrary m_shaderProgLibrary;

			ID3D11DepthStencilView *m_depthStencilView;
			ID3D11RenderTargetView *m_renderTargetView;
	};
}
