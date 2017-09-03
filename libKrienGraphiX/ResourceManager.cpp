
#include <comdef.h>
#include <iostream>

#include "Filesystem.h"
#include "ShaderProgram.h"
#include "TextureManager.h"
#include "VertexInputLayout.h"

#include "parsers/KGMaterialLibraryParser.h"
#include "parsers/KGShaderProgramsParser.h"

#include "ResourceManager.h"

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
			m_materials(), m_nextShaderProgramID(0u), m_shaderPrograms()
	{
		loadDefaultShaderPrograms();
	}

	ResourceManager::~ResourceManager()
	{
		clearResources();
	}


	MeshBuffer ResourceManager::getMeshBuffer( MeshBufferID id ) const
	{
		std::unordered_map<MeshBufferID, MeshBuffer>::const_iterator it;
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
			vertBuff = nullptr;

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
		std::unordered_map<MeshBufferID, MeshBuffer>::iterator meshIt;
		meshIt = m_meshBuffers.find(id);

		if ( meshIt != m_meshBuffers.end() )
		{
			if ( meshIt->second.vertBuff )
				meshIt->second.vertBuff->Release();
			if ( meshIt->second.indexBuff )
				meshIt->second.indexBuff->Release();

			m_meshBuffers.erase(id);
		}
	}

	Material ResourceManager::getMaterial( const std::string &matName )
	{
		if ( loadMaterials() )
		{
			std::unordered_map<std::string, Material>::const_iterator it;
			it = m_materials.find( matName );

			if ( it != m_materials.cend() )
				return it->second;
		}

		return Material();
	}

	ShaderProgram* ResourceManager::getShaderProgram( const std::string &name ) const
	{
		std::unordered_map<std::string, ShaderProgram::ShaderProgramID>::const_iterator it;
		it = m_shaderProgramNames.find( name );

		if ( it != m_shaderProgramNames.cend() )
			return getShaderProgram(it->second);

		return nullptr;
	}
	ShaderProgram* ResourceManager::getShaderProgram( ShaderProgram::ShaderProgramID id ) const
	{
		std::unordered_map<ShaderProgram::ShaderProgramID, ShaderProgram*>::const_iterator it;
		it = m_shaderPrograms.find( id );

		if ( it != m_shaderPrograms.cend() )
			return it->second;

		return nullptr;
	}

	ShaderProgram* ResourceManager::createShaderProgram( const std::string &name )
	{
		ShaderProgram *shaderProg = getShaderProgram( name );
		if ( shaderProg )
		{
			std::cout << "Warning (ResourceManager::createShaderProgram): ShaderProgram with name " << name << " already exists." << std::endl;
			return shaderProg;
		}

		shaderProg = new ShaderProgram( m_dxDev, m_nextShaderProgramID, name );

		m_shaderPrograms.insert( std::pair<ShaderProgram::ShaderProgramID, ShaderProgram*>( m_nextShaderProgramID, shaderProg ) );
		m_shaderProgramNames.insert( std::pair<std::string, ShaderProgram::ShaderProgramID>( name, m_nextShaderProgramID ) );
		++m_nextShaderProgramID;

		return shaderProg;
	}

	void ResourceManager::releaseShaderProgram( const std::string &name )
	{
		ShaderProgram *sp = getShaderProgram( name );
		if ( sp )
			releaseShaderProgram( sp->getID() );
	}
	void ResourceManager::releaseShaderProgram( ShaderProgram::ShaderProgramID id )
	{
		std::unordered_map<ShaderProgram::ShaderProgramID, ShaderProgram*>::iterator it;
		it = m_shaderPrograms.find( id );

		if ( it != m_shaderPrograms.end() )
		{
			// remove from name-id map
			m_shaderProgramNames.erase( it->second->getName() );

			// remove from id-pointer map
			delete it->second;
			m_shaderPrograms.erase( it );
		}
	}


	void ResourceManager::clearResources( bool reloadDefaultShaderPrograms )
	{
		// release all vertex- and index buffers
		std::unordered_map<MeshBufferID, MeshBuffer>::iterator bIt;
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

		// release all ShaderPrograms
		std::unordered_map<ShaderProgram::ShaderProgramID, ShaderProgram*>::iterator spIt;
		for ( spIt = m_shaderPrograms.begin(); spIt != m_shaderPrograms.end(); ++spIt )
			delete spIt->second;
		m_shaderPrograms.clear();
		m_shaderProgramNames.clear();

		m_nextShaderProgramID  = 0u;

		// reload default shader programs if requested
		if ( reloadDefaultShaderPrograms )
			loadDefaultShaderPrograms();
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

	bool ResourceManager::loadDefaultShaderPrograms()
	{
		// Parse and load the shader programs
		std::string shaderProgString;
		if ( filesystem::openFile( "ShaderPrograms.kgsp", shaderProgString ) )
		{
			std::unordered_map<std::string, KgShaderProgData> shaderProgData;
			KGShaderProgramsParser::parse( shaderProgString, shaderProgData );

			std::unordered_map<std::string, KgShaderProgData>::const_iterator it;
			for ( it = shaderProgData.cbegin(); it != shaderProgData.cend(); ++it )
			{
				ShaderProgram *shaderProg = createShaderProgram( it->first );
				if ( !it->second.vertexShader.empty() && !it->second.vertLayoutTypes.empty() )
					shaderProg->createVertexShader( it->second.vertexShader, VertexInputLayout( it->second.vertLayoutTypes ) );

				if ( !it->second.pixelShader.empty() )
					shaderProg->createPixelShader( it->second.pixelShader );
			}

			return true;
		}

		return false;
	}
}
