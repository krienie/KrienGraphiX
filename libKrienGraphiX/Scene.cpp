
#include <d3d11.h>

#include "Camera.h"
#include "PixelShader.h"
#include "RenderCommand.h"
#include "RenderBucket.h"
#include "RenderableObject.h"
#include "ResourceManager.h"
#include "ShaderProgram.h"
#include "TextureManager.h"
#include "VertexShader.h"
#include "Scene.h"

namespace kgx
{
	Scene::Scene()
		: m_lightData(), m_nextCamID(0u), m_defaultCamera(nullptr), m_cameras(), m_renderObjects()
	{
		m_lightData.ambientLight = DirectX::XMFLOAT4( 0.25f, 0.25f, 0.25f, 1.0f );
	}

	Scene::~Scene()
	{
		std::map<CameraID, Camera*>::iterator camIt;
		for ( camIt = m_cameras.begin(); camIt != m_cameras.end(); ++camIt )
			delete camIt->second;

		//TODO: release RenderObject data
	}


	Scene::CameraID Scene::createCamera( const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up )
	{
		return createCamera( DirectX::XM_PIDIV4, 1.0f, 0.001f, 500.0f, eye, target, up );
	}
	Scene::CameraID Scene::createCamera( float fovY, float aspect, float nearZ, float farZ,
					   const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up )
	{
		Camera *newCam = new Camera( this, fovY, aspect, nearZ, farZ, eye, target, up );
		m_cameras.insert( std::pair<CameraID, Camera*>(m_nextCamID, newCam) );
		++m_nextCamID;

		if ( !m_defaultCamera )
			m_defaultCamera = newCam;

		return m_nextCamID - 1u;
	}


	Scene::const_cameraiterator Scene::getCameraCBegin() const
	{
		return m_cameras.cbegin();
	}
	Scene::const_cameraiterator Scene::getCameraCEnd() const
	{
		return m_cameras.cend();
	}
	
	Scene::const_renobjectiterator Scene::getRenObjectCBegin() const
	{
		return m_renderObjects.cbegin();
	}
	Scene::const_renobjectiterator Scene::getRenObjectCEnd() const
	{
		return m_renderObjects.cend();
	}

	Scene::const_lightiterator Scene::getLightCBegin() const
	{
		return m_lightData.lights.cbegin();
	}
	Scene::const_lightiterator Scene::getLightCEnd() const
	{
		return m_lightData.lights.cend();
	}


	Camera* Scene::getCamera( CameraID id ) const
	{
		std::map<CameraID, Camera*>::const_iterator it = m_cameras.find( id );
		if ( it != m_cameras.end() )
			return it->second;

		return nullptr;
	}
	Camera* Scene::getDefaultCamera() const
	{
		return m_defaultCamera;
	}
	void Scene::setDefaultCamera( CameraID id )
	{
		m_defaultCamera = getCamera( id );
	}

	DirectX::XMFLOAT3 Scene::getAmbient() const
	{
		return DirectX::XMFLOAT3( m_lightData.ambientLight.x, m_lightData.ambientLight.y, m_lightData.ambientLight.z );
	}

	void Scene::setAmbient( const DirectX::XMFLOAT3 &color )
	{
		m_lightData.ambientLight = DirectX::XMFLOAT4( color.x, color.y, color.z, 1.0f );
	}
	void Scene::setAmbient( float r, float g, float b )
	{
		m_lightData.ambientLight = DirectX::XMFLOAT4( r, g, b, 1.0f );
	}

	void Scene::addDirectionalLight( const DirectX::XMFLOAT3 &direction, float intensity )
	{
		// push a Light struct in the vector
		m_lightData.lights.push_back( { direction, intensity } );
	}

	void Scene::addRenderableObject( const RenderableObject &obj )
	{
		m_renderObjects.push_back(obj);
	}


	void Scene::render( Camera *renderCam, ID3D11RenderTargetView *rtv, ID3D11DepthStencilView *dsv )
	{
		RenderBucket mainRenderBucket( rtv, dsv, renderCam->getViewMatrix(), renderCam->getProjMatrix() );

		std::vector<RenderableObject>::iterator it;
		for ( it = m_renderObjects.begin(); it != m_renderObjects.end(); ++it )
		{
			UINT commandKey = createCommandKey( *it );

			ShaderProgram *shaderProg = ResourceManager::getInst()->getShaderProgram( it->shaderProgram );

			// set model/normal matrix
			rendercommand::CopyConstantBufferData *modelNormalCommand = mainRenderBucket.addCommand<rendercommand::CopyConstantBufferData>( commandKey, sizeof(DirectX::XMFLOAT4X4) * 2u );
			modelNormalCommand->constantBuffer = shaderProg->getVertexShader()->getConstantBufferPtrByIndex(1);		//TODO: make this part more generic/flexible
			modelNormalCommand->size           = sizeof(DirectX::XMFLOAT4X4) * 2u;
			modelNormalCommand->data           = rendercommandpacket::getAuxiliaryMemory( modelNormalCommand );
			DirectX::XMFLOAT4X4 matrices[2] = { getModelMatrix(*it), getNormalMatrix(*it) };
			memcpy( modelNormalCommand->data, matrices, sizeof(DirectX::XMFLOAT4X4) * 2u );

			// set material constants and shader resources (textures and such)
			rendercommand::CopyConstantBufferData *materialCommand = mainRenderBucket.appendCommand<rendercommand::CopyConstantBufferData>( modelNormalCommand, sizeof(DirectX::XMFLOAT4) * 2u );
			materialCommand->constantBuffer = shaderProg->getPixelShader()->getConstantBufferPtrByIndex(2);
			materialCommand->size           = sizeof(DirectX::XMFLOAT4) * 2u;
			materialCommand->data           = rendercommandpacket::getAuxiliaryMemory( materialCommand );
			DirectX::XMFLOAT4 matData[2] = { it->material.diffuse, it->material.specular };
			memcpy( materialCommand->data, matData, sizeof(DirectX::XMFLOAT4) * 2u );

			// set default texture sampler
			rendercommand::SetPixelShaderSamplers *samplerCommand = mainRenderBucket.appendCommand<rendercommand::SetPixelShaderSamplers>( materialCommand, sizeof(ID3D11SamplerState*) * 1u );
			samplerCommand->startSlot   = 0u;
			samplerCommand->numSamplers = 1u;

			samplerCommand->samplers = reinterpret_cast<ID3D11SamplerState**>( rendercommandpacket::getAuxiliaryMemory(samplerCommand) );
			ID3D11SamplerState *samplers[1] = { TextureManager::getInst()->getDefaultSampler() };
			memcpy( samplerCommand->samplers, samplers, sizeof(ID3D11SamplerState*) * 1u );


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
			rendercommand::SetPixelShaderResourceViews *texCommand = mainRenderBucket.appendCommand<rendercommand::SetPixelShaderResourceViews>( samplerCommand, sizeof(ID3D11ShaderResourceView*) * texViews.size() );
			texCommand->startSlot = 0u;
			texCommand->numViews  = static_cast<UINT>(texViews.size());

			texCommand->views = reinterpret_cast<ID3D11ShaderResourceView**>( rendercommandpacket::getAuxiliaryMemory(texCommand) );
			memcpy( texCommand->views, texViews.data(), sizeof(ID3D11ShaderResourceView*) * texViews.size() );

			

			// bind mesh to IA stage
			rendercommand::BindMeshToIAStage *bindMeshCommand = mainRenderBucket.appendCommand<rendercommand::BindMeshToIAStage>( texCommand, 0u );
			bindMeshCommand->primitiveTopology = it->topology;
			bindMeshCommand->meshBuffer        = it->meshBuffer;

			// draw indexed
			rendercommand::DrawIndexed *drawCommand = mainRenderBucket.appendCommand<rendercommand::DrawIndexed>( bindMeshCommand, 0u );
			drawCommand->baseVertex = it->baseVertex;
			drawCommand->startIndex = it->startIndex;
			drawCommand->indexCount = it->indexCount;

			//TODO: maybe merge BindMeshToIAStage command into DrawIndexed command
		}

		mainRenderBucket.submit( m_lightData );
	}

	UINT Scene::createCommandKey( const RenderableObject &obj ) const
	{
		// for now, the command key for an RenderableObject is a combination of the ShaderProgramID and the MeshBufferID
		return (obj.shaderProgram & 0xFFFF) << 16 | (obj.meshBuffer & 0xFFFF);
	}
}
