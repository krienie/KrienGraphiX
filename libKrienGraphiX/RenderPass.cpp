
#define NOMINMAX
#include <limits>

#include "KGXCore.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "PhysXManager.h"
#include "ResourceManager.h"
#include "TextureManager.h"
#include "RenderPass.h"

namespace kgx
{
	RenderPass::RenderPass( ID3D11DeviceContext *defDevCont, ID3D11RenderTargetView *rtv, ID3D11DepthStencilView *dsv,
								const DirectX::XMFLOAT4X4 &viewMatrix, const DirectX::XMFLOAT4X4 &projMatrix,
								ShaderProgram::ShaderProgramID shaderProgID )
		: m_dxDevCont(nullptr), m_dxDeferredDevCont(defDevCont), m_commandList(nullptr),
			m_renderTargetView(rtv), m_depthStencilView(dsv), m_viewMatrix(viewMatrix), m_projMatrix(projMatrix),
			m_shaderProgID(shaderProgID)
	{
		ID3D11Device *dxDev = KGXCore::getInst()->getDxDevicePtr();
		dxDev->GetImmediateContext( &m_dxDevCont );
	}

	RenderPass::~RenderPass()
	{
		if ( m_dxDevCont )
			m_dxDevCont->Release();
		if ( m_commandList )
			m_commandList->Release();
	}

	void RenderPass::record( const std::vector<RenderableObject> &renderObjects, const LightData &lightData )
	{
		m_dxDeferredDevCont->OMSetRenderTargets( 1, &m_renderTargetView, m_depthStencilView );

		// set per frame data
		ShaderProgram *shaderProg = ResourceManager::getInst()->getShaderProgram( m_shaderProgID );
		// We always expect there to be a shader program. If no shader program is found, the program is allowed to crash.
		shaderProg->activate( m_dxDeferredDevCont );		// bind shaders to pipeline

		// set per camera data
		ConstantBuffer *vertexPerCamBuffer = shaderProg->getVertexShader()->getConstantBufferPtrByIndex( 0 );
		DirectX::XMFLOAT4X4 perCamData[2] = { m_viewMatrix, m_projMatrix };
		vertexPerCamBuffer->copyBufferData( perCamData, sizeof(DirectX::XMFLOAT4X4) * 2u, m_dxDeferredDevCont );

		struct TempLightData
		{
			DirectX::XMFLOAT4 ambientLight;
			Light lights;
		} tmpLightData;

		tmpLightData.ambientLight = lightData.ambientLight;
		tmpLightData.lights = lightData.lights[0];

		// set light data
		ConstantBuffer *pixelLightDataBuffer = shaderProg->getPixelShader()->getConstantBufferPtrByIndex( 0 );
		pixelLightDataBuffer->copyBufferData( &tmpLightData, sizeof( TempLightData ), m_dxDeferredDevCont );

		std::vector<RenderableObject>::const_iterator it;
		for ( it = renderObjects.begin(); it != renderObjects.end(); ++it )
		{
			// get model/normal matrix
			DirectX::XMFLOAT4X4 modelMatrix;
			if ( !PhysXManager::getInst()->getShapeGlobalPosition( it->name, modelMatrix ) )
				modelMatrix = getModelMatrix( *it );

			DirectX::XMFLOAT4X4 matrices[2] = { modelMatrix, getNormalMatrix( *it ) };

			// set model/normal matrix
			ConstantBuffer *vertexPerObjBuffer = shaderProg->getVertexShader()->getConstantBufferPtrByIndex( 1 );
			vertexPerObjBuffer->copyBufferData( matrices, sizeof(DirectX::XMFLOAT4X4) * 2u, m_dxDeferredDevCont );


			// get material constants and shader resources (textures and such)
			int useTextures = static_cast<int>(it->material.diffuseMap >= 0);
			DirectX::XMFLOAT4 useTextures4;
			useTextures4.x = static_cast<float>(useTextures);

			DirectX::XMFLOAT4 matData[3] = { it->material.diffuse, it->material.specular, useTextures4 };

			// get material constants and shader resources (textures and such)
			ConstantBuffer *pixelPerObjectData = shaderProg->getPixelShader()->getConstantBufferPtrByIndex( 2 );
			pixelPerObjectData->copyBufferData( matData, sizeof(DirectX::XMFLOAT4) * 3u, m_dxDeferredDevCont );


			// set default texture sampler
			ID3D11SamplerState *samplers[1] = { TextureManager::getInst()->getDefaultSampler() };
			m_dxDeferredDevCont->PSSetSamplers( 0u, 1u, samplers );

			// get textures
			std::vector<Texture::TextureID> textures;
			if ( it->material.diffuseMap >= 0 )
				textures.push_back( it->material.diffuseMap );
			if ( it->material.specularMap >= 0 )
				textures.push_back( it->material.specularMap );
			if ( it->material.normalMap >= 0 )
				textures.push_back( it->material.normalMap );

			// collect all ShaderResourceViews for all used textures
			std::vector<ID3D11ShaderResourceView*> texViews;
			texViews.reserve( textures.size() );

			std::vector<Texture::TextureID>::const_iterator texIt;
			for ( texIt = textures.begin(); texIt != textures.end(); ++texIt )
			{
				Texture *texture = TextureManager::getInst()->getTexture( *texIt );
				texViews.push_back( texture->getResourceView() );		// assumes the texture is always found..
			}

			// set texture ShaderResourceViews for the pixel shader
			m_dxDeferredDevCont->PSSetShaderResources( 0u, static_cast<UINT>(texViews.size()), texViews.data() );

			// bind Vertex- and Index-buffers to IA Stage
			MeshBuffer buff = ResourceManager::getInst()->getMeshBuffer( it->meshBuffer );

			// bind Vertex- and Index-buffers to IA Stage
			UINT bufferStride = buff.vertexBufferStride;
			UINT offset = 0;
			m_dxDeferredDevCont->IASetVertexBuffers( 0, 1, &buff.vertBuff, &bufferStride, &offset );
			m_dxDeferredDevCont->IASetIndexBuffer( buff.indexBuff, DXGI_FORMAT_R32_UINT, 0 );
			m_dxDeferredDevCont->IASetPrimitiveTopology( it->topology );

			// draw indexed
			m_dxDeferredDevCont->DrawIndexed( it->indexCount, it->startIndex, it->baseVertex );
		}

		// finish commandlist
		if ( m_commandList )
			m_commandList->Release();		// this prob never happens, if RenderPass is used properly

		m_dxDeferredDevCont->FinishCommandList( false, &m_commandList );
	}

	void RenderPass::submit()
	{
		if ( m_commandList )
			m_dxDevCont->ExecuteCommandList( m_commandList, false );
	}
}
