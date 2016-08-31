
#include <comdef.h>
#include <iostream>

#include "Filesystem.h"
#include "ShaderProgram.h"
#include "ResourceManager.h"
#include "TextureManager.h"
#include "VertexInputLayout.h"

#include "parsers\KGMaterialLibraryParser.h"

namespace kgx
{
	ResourceManager* ResourceManager::m_inst = nullptr;

	ResourceManager* ResourceManager::getInst()
	{
		return m_inst;
	}

	void ResourceManager::construct( ID3D11Device *m_dxDev )
	{
		if ( ResourceManager::m_inst )
			delete ResourceManager::m_inst;

		m_inst = new ResourceManager( m_dxDev );
	}


	void ResourceManager::destroy()
	{
		if ( ResourceManager::m_inst )
			delete ResourceManager::m_inst;
	}


	ResourceManager::ResourceManager( ID3D11Device *dxDevice )
		: m_dxDev(dxDevice), m_nextBufferID(0u), m_meshBuffers(),
			m_materials(), m_nextShaderProgramID(0u), m_defaultShaderProgram(-1), m_shaderPrograms()
	{
	}

	ResourceManager::~ResourceManager()
	{
		clearResources();
	}


	MeshBuffer ResourceManager::getMeshBuffer( MeshBufferID id ) const
	{
		std::map<MeshBufferID, MeshBuffer>::const_iterator it;
		it = m_meshBuffers.find(id);

		if ( it != m_meshBuffers.cend() )
			return it->second;

		std::cout << "Warning (ResourceManager::getBuffer): buffer with id " << id << " was not found." << std::endl;
		return MeshBuffer();
	}

	MeshBufferID ResourceManager::addMeshBuffer( const std::vector<float> &vertices, const std::vector<UINT> &indices,
																	const VertexInputLayout &inputDescriptor, HRESULT &result )
	{
		if ( vertices.size() <= 0 || indices.size() <= 0 )
		{
			std::cout << "Error (ResourceManager::addMeshBuffer): Vertices or indices not specified => vector is empty" << std::endl;
			result = E_UNEXPECTED;
			return 0;
		}

		D3D11_SUBRESOURCE_DATA vertResource;
		vertResource.pSysMem          = vertices.data();
		vertResource.SysMemPitch      = 0;
		vertResource.SysMemSlicePitch = 0;

		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory( &buffDesc, sizeof(buffDesc) );

		buffDesc.ByteWidth = sizeof(float) * UINT(vertices.size());
		buffDesc.Usage     = D3D11_USAGE_IMMUTABLE;
		buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		ID3D11Buffer *vertBuff;
		result = m_dxDev->CreateBuffer( &buffDesc, &vertResource, &vertBuff );

		if ( FAILED(result) )
		{
			_com_error error(result);
			std::cout << "Error (ResourceManager::addMeshBuffer): " << error.ErrorMessage() << std::endl;
			return 0;
		}

		buffDesc.ByteWidth = sizeof(UINT) * UINT(indices.size());
		buffDesc.Usage     = D3D11_USAGE_IMMUTABLE;
		buffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexResource;
		indexResource.pSysMem          = indices.data();
		indexResource.SysMemPitch      = 0;
		indexResource.SysMemSlicePitch = 0;

		ID3D11Buffer *indexBuff;
		result = m_dxDev->CreateBuffer( &buffDesc, &indexResource, &indexBuff );

		if ( FAILED(result) )
		{
			vertBuff->Release();
			vertBuff = NULL;

			_com_error error(result);
			std::cout << "Error (ResourceManager::addMeshBuffer): " << error.ErrorMessage() << std::endl;
			return 0;
		}


		// add buffers to buffers list
		MeshBuffer mBuff;
		mBuff.vertBuff           = vertBuff;
		mBuff.indexBuff          = indexBuff;
		mBuff.vertexBufferStride = inputDescriptor.getBufferStride();

		m_meshBuffers.insert( std::pair<MeshBufferID, MeshBuffer>(m_nextBufferID, mBuff) );
		++m_nextBufferID;

		result = S_OK;

		return m_nextBufferID - 1u;
	}

	void ResourceManager::releaseBuffer( MeshBufferID id )
	{
		std::map<MeshBufferID, MeshBuffer>::iterator it;
		it = m_meshBuffers.find(id);

		if ( it != m_meshBuffers.end() )
		{
			if ( it->second.vertBuff )
				it->second.vertBuff->Release();
			if ( it->second.indexBuff )
				it->second.indexBuff->Release();

			m_meshBuffers.erase(id);
		}
	}


	Material ResourceManager::getMaterial( const std::string &matName )
	{
		if ( loadMaterials() )
		{
			std::map<std::string, Material>::const_iterator it;
			it = m_materials.find( matName );

			if ( it != m_materials.cend() )
				return it->second;
		}

		return Material();
	}

	ShaderProgram::ShaderProgramID ResourceManager::getDefaultShaderProgram()
	{
		if ( m_defaultShaderProgram >= 0 )
			return m_defaultShaderProgram;

		ShaderProgram *prog = createShaderProgram();
		m_defaultShaderProgram = prog->getID();

		VertexInputLayout vertInputLayout;
		vertInputLayout.addInputType( VertexInputLayout::Position );
		vertInputLayout.addInputType( VertexInputLayout::TextureCoordinate );
		vertInputLayout.addInputType( VertexInputLayout::Normal );
		prog->createVertexShader( "DefaultVS.cso", vertInputLayout );

		prog->createPixelShader( "DefaultPS.cso" );


		return m_defaultShaderProgram;
	}


	ShaderProgram* ResourceManager::getShaderProgram( ShaderProgram::ShaderProgramID id ) const
	{
		std::map<ShaderProgram::ShaderProgramID, ShaderProgram*>::const_iterator it;
		it = m_shaderPrograms.find( id );

		if ( it != m_shaderPrograms.cend() )
			return it->second;

		std::cout << "Warning (ResourceManager::getShaderProgram): ShaderProgram with id " << id << " was not found." << std::endl;
		return nullptr;
	}

	ShaderProgram* ResourceManager::createShaderProgram()
	{
		ShaderProgram *newProg = new ShaderProgram( m_dxDev, m_nextShaderProgramID );

		m_shaderPrograms.insert( std::pair<ShaderProgram::ShaderProgramID, ShaderProgram*>( m_nextShaderProgramID, newProg ) );
		++m_nextShaderProgramID;

		return newProg;
	}

	void ResourceManager::releaseShaderProgram( ShaderProgram::ShaderProgramID id )
	{
		std::map<ShaderProgram::ShaderProgramID, ShaderProgram*>::iterator it;
		it = m_shaderPrograms.find( id );

		if ( it != m_shaderPrograms.end() )
			delete it->second;
	}


	void ResourceManager::clearResources()
	{
		// release all vertex- and index buffers
		std::map<MeshBufferID, MeshBuffer>::iterator bIt;
		for ( bIt = m_meshBuffers.begin(); bIt != m_meshBuffers.end(); ++bIt )
		{
			if ( bIt->second.vertBuff )
				bIt->second.vertBuff->Release();
			if ( bIt->second.indexBuff )
				bIt->second.indexBuff->Release();
		}
		m_meshBuffers.clear();

		m_nextBufferID = 0u;

		// clear all materials
		m_materials.clear();

		// release all ShaderProgram
		std::map<ShaderProgram::ShaderProgramID, ShaderProgram*>::iterator mIt;
		for ( mIt = m_shaderPrograms.begin(); mIt != m_shaderPrograms.end(); ++mIt )
			delete mIt->second;
		m_shaderPrograms.clear();

		m_defaultShaderProgram = -1;
		m_nextShaderProgramID  = 0u;
	}

	bool ResourceManager::loadMaterials()
	{
		if ( !m_materials.empty() )
			return true;

		std::string matLibraryString;
		if ( filesystem::openFile( "MaterialLibrary.kgmat", matLibraryString ) )
		{
			std::map<std::string, KgMatData> materials;
			KGMaterialLibraryParser::parse( matLibraryString, materials );

			std::map<std::string, KgMatData>::const_iterator it;
			for ( it = materials.cbegin(); it != materials.cend(); ++it )
			{
				Material mat;
				mat.diffuse  = it->second.diffuse;
				mat.specular = it->second.specular;

				if ( it->second.diffuseMap.size() > 0 )
					mat.diffuseMap = TextureManager::getInst()->loadTexture( it->second.diffuseMap );
				else mat.diffuseMap = -1;

				if ( it->second.specularMap.size() > 0 )
					mat.specularMap = TextureManager::getInst()->loadTexture( it->second.specularMap );
				else mat.specularMap = -1;

				if ( it->second.normalMap.size() > 0 )
					mat.normalMap = TextureManager::getInst()->loadTexture( it->second.normalMap );
				else mat.normalMap = -1;

				m_materials.insert( std::pair<std::string, Material>(it->first, mat) );
			}

			return true;
		}

		return false;
	}
}
