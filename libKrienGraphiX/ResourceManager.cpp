
#include <comdef.h>
#include <iostream>

#include "Material.h"
#include "ResourceManager.h"

namespace kgx
{
	ResourceManager* ResourceManager::m_inst = 0;

	ResourceManager* ResourceManager::getInst()
	{
		return m_inst;
	}

	void ResourceManager::construct( _In_ ID3D11Device *m_dxDev )
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


	ResourceManager::ResourceManager( _In_ ID3D11Device *dxDevice )
		: m_dxDev(dxDevice), m_nextBufferID(1), m_meshBuffers(), m_nextMaterialID(1), m_materials()
	{
	}

	ResourceManager::~ResourceManager()
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

		// release all material
		std::map<MaterialID, Material*>::iterator mIt;
		for ( mIt = m_materials.begin(); mIt != m_materials.end(); ++mIt )
			delete mIt->second;
	}


	MeshBuffer ResourceManager::getBuffer( MeshBufferID id ) const
	{
		std::map<MeshBufferID, MeshBuffer>::const_iterator it;
		it = m_meshBuffers.find(id);

		if ( it != m_meshBuffers.cend() )
			return it->second;

		std::cout << "Warning (ResourceManager::getBuffer): buffer with id " << id << " was not found." << std::endl;
		return MeshBuffer();
	}

	ResourceManager::MeshBufferID ResourceManager::addMeshBuffer( const std::vector<float> &vertices, const std::vector<UINT> &indices,
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
		buffDesc.Usage     = D3D11_USAGE_DEFAULT;
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
		buffDesc.Usage     = D3D11_USAGE_DEFAULT;
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
		mBuff.vertBuff        = vertBuff;
		mBuff.indexBuff       = indexBuff;
		mBuff.inputDescriptor = inputDescriptor;

		m_meshBuffers.insert( std::pair<MeshBufferID, MeshBuffer>(m_nextBufferID, mBuff) );
		m_nextBufferID++;

		result = S_OK;

		return m_nextBufferID - 1;
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


	Material* ResourceManager::getMaterial( MaterialID id ) const
	{
		std::map<MaterialID, Material*>::const_iterator it;
		it = m_materials.find(id);

		if ( it != m_materials.cend() )
			return it->second;

		std::cout << "Warning (ResourceManager::getMaterial): Material with id " << id << " was not found." << std::endl;
		return nullptr;
	}

	ResourceManager::MaterialID ResourceManager::claimMaterial( _In_ Material *mat )
	{
		m_materials.insert( std::pair<MaterialID, Material*>(m_nextMaterialID, mat) );
		m_nextMaterialID++;

		return m_nextMaterialID - 1;
	}

	void ResourceManager::releaseMaterial( MaterialID id )
	{
		std::map<MaterialID, Material*>::iterator it;
		it = m_materials.find(id);

		if ( it != m_materials.end() )
			delete it->second;
	}
}
