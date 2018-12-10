
#include "Rendering/Passes/GBufferPass.h"

#include "Core/PhysXManager.h"
#include "Simulation/Camera.h"
#include "Rendering/ConstantBuffer.h"
#include "Rendering/MeshBuffer.h"
#include "Rendering/RenderTexture.h"
#include "Rendering/ShaderProgram.h"
#include "Rendering/Shaders/VertexShader.h"
#include "Rendering/Shaders/PixelShader.h"

namespace kgx
{
	GBufferPass::GBufferPass( ID3D11DeviceContext *deferredDevCont, ID3D11DepthStencilView *dsv,
								  Camera *renderCam, unsigned int width, unsigned int height )
		: RenderPass(deferredDevCont), m_gbuffTextureLib(), m_gbuffTextures(), m_shaderProg(nullptr),
			m_shaderProgLibrary(true), m_depthStencilView(dsv), m_renderCam(renderCam)
	{
		m_gbuffTextures.push_back( m_gbuffTextureLib.createRenderTexture(width, height) );
		m_gbuffTextures.push_back( m_gbuffTextureLib.createRenderTexture(width, height) );

		m_shaderProg = m_shaderProgLibrary.getShaderProgram("DeferredGBuffer");
	}

	GBufferPass::~GBufferPass()
	{
	}

	void GBufferPass::record( const std::vector<RenderableObject*> &renderObjects, const LightData &lightData,
							  const TextureLibrary &sceneTextures )
	{
		ID3D11DeviceContext *dxDeferredDevCont = getDeferredContext();

		// Diffuse render texture
		ID3D11RenderTargetView *diffuseBuffView = m_gbuffTextures[0]->getRenderTargetView();
		float m_diffuseClearColor[4] = {0.4f, 0.4f, 0.4f, 1.0f};
		dxDeferredDevCont->ClearRenderTargetView( diffuseBuffView, m_diffuseClearColor );

		// Depth render texture
		ID3D11RenderTargetView *depthBuffView = m_gbuffTextures[1]->getRenderTargetView();
		float m_depthClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		dxDeferredDevCont->ClearRenderTargetView( depthBuffView, m_depthClearColor );

		// Set output render target views
		ID3D11RenderTargetView *rtvOutput[2] = { diffuseBuffView, depthBuffView };
		dxDeferredDevCont->OMSetRenderTargets( 2, rtvOutput, m_depthStencilView );

		// set per frame data
		m_shaderProg->activate( dxDeferredDevCont );		// bind shaders to pipeline
		
		// set per camera data
		ConstantBuffer *vertexPerCamBuffer = m_shaderProg->getVertexShader()->getConstantBufferPtrByIndex( 0 );
		DirectX::XMFLOAT4X4 perCamData[2] = { m_renderCam->getViewMatrix(), m_renderCam->getProjMatrix() };
		vertexPerCamBuffer->copyBufferData( perCamData, sizeof( DirectX::XMFLOAT4X4 ) * 2u, dxDeferredDevCont );

		std::vector<RenderableObject*>::const_iterator it;
		for ( it = renderObjects.begin(); it != renderObjects.end(); ++it )
		{
			// get model/normal matrix
			DirectX::XMFLOAT4X4 modelMatrix;
			if ( !PhysXManager::getInst()->getShapeGlobalPosition( (*it)->getName(), modelMatrix ) )
				modelMatrix = (*it)->getModelMatrix();

			DirectX::XMFLOAT4X4 matrices[2] = { modelMatrix, (*it)->getNormalMatrix() };

			// set model/normal matrix
			ConstantBuffer *vertexPerObjBuffer = m_shaderProg->getVertexShader()->getConstantBufferPtrByIndex( 1 );
			vertexPerObjBuffer->copyBufferData( matrices, sizeof( DirectX::XMFLOAT4X4 ) * 2u, dxDeferredDevCont );


			// get material constants and shader resources (textures and such)
			Material mat = (*it)->getMaterial();
			int useTextures = static_cast<int>(!mat.diffuseMap.empty());
			DirectX::XMFLOAT4 useTextures4;
			useTextures4.x = static_cast<float>(useTextures);

			DirectX::XMFLOAT4 matData[3] = { mat.diffuse, mat.specular, useTextures4 };

			// get material constants and shader resources (textures and such)
			ConstantBuffer *pixelPerObjectData = m_shaderProg->getPixelShader()->getConstantBufferPtrByIndex( 0 );
			pixelPerObjectData->copyBufferData( matData, sizeof( DirectX::XMFLOAT4 ) * 3u, dxDeferredDevCont );

			// set default texture sampler
			ID3D11SamplerState *samplers[1] = { m_gbuffTextureLib.getDefaultSampler() };
			dxDeferredDevCont->PSSetSamplers( 0u, 1u, samplers );

			// get textures
			std::vector<std::string> textures;
			if ( !mat.diffuseMap.empty() )
				textures.push_back( mat.diffuseMap );
			if ( !mat.specularMap.empty() )
				textures.push_back( mat.specularMap );
			if ( !mat.normalMap.empty() )
				textures.push_back( mat.normalMap );

			// collect all ShaderResourceViews for all used textures
			std::vector<ID3D11ShaderResourceView*> texViews;
			texViews.reserve( textures.size() );

			std::vector<std::string>::const_iterator texIt;
			for ( texIt = textures.cbegin(); texIt != textures.cend(); ++texIt )
			{
				Texture *texture = sceneTextures.getTexture( *texIt );
				texViews.push_back( texture->getResourceView() );		// assumes the texture is always found..
			}

			// set texture ShaderResourceViews for the pixel shader
			dxDeferredDevCont->PSSetShaderResources( 0u, static_cast<UINT>(texViews.size()), texViews.data() );

			// bind Vertex- and Index-buffers to IA Stage
			const MeshBuffer *meshBuff = (*it)->getMeshBuffer();
			UINT bufferStride = meshBuff->getVertexBufferStride();
			UINT offset = 0;

			ID3D11Buffer *vertexBuffer = meshBuff->getVertexBuffer();
			dxDeferredDevCont->IASetVertexBuffers( 0, 1, &vertexBuffer, &bufferStride, &offset );
			dxDeferredDevCont->IASetIndexBuffer( meshBuff->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0 );
			dxDeferredDevCont->IASetPrimitiveTopology( (*it)->getTopology() );

			// draw indexed
			dxDeferredDevCont->DrawIndexed( (*it)->getIndexCount(), (*it)->getStartIndex(), (*it)->getBaseVertex() );
		}

		finishCommandList();
	}

	const TextureLibrary& GBufferPass::getTextureLibrary() const
	{
		return m_gbuffTextureLib;
	}
}
