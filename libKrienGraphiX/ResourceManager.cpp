
#include <comdef.h>
#include <iostream>

#include "ResourceManager.h"

namespace kgx
{
	ResourceManager* ResourceManager::inst = 0;

	ResourceManager* ResourceManager::getInst()
	{
		return inst;
	}

	void ResourceManager::construct( _In_ ID3D11Device *dxDevice )
	{
		if ( ResourceManager::inst )
			delete ResourceManager::inst;

		inst = new ResourceManager( dxDevice );
	}


	void ResourceManager::destroy()
	{
		if ( ResourceManager::inst )
			delete ResourceManager::inst;
	}


	ResourceManager::ResourceManager( _In_ ID3D11Device *dxDev )
		: dxDevice(dxDev), nextBufferID(1), meshBuffers(), nextMaterialID(1), materials()
	{
	}

	ResourceManager::~ResourceManager()
	{
		// release all vertex- and index buffers
		std::map<MeshBufferID, MeshBuffer>::iterator bIt;
		for ( bIt = meshBuffers.begin(); bIt != meshBuffers.end(); ++bIt )
		{
			if ( bIt->second.vertBuff )
				bIt->second.vertBuff->Release();
			if ( bIt->second.indexBuff )
				bIt->second.indexBuff->Release();
		}

		// release all material
		std::map<MaterialID, Material*>::iterator mIt;
		for ( mIt = materials.begin(); mIt != materials.end(); ++mIt )
			delete mIt->second;
	}


	MeshBuffer ResourceManager::getBuffer( MeshBufferID id ) const
	{
		std::map<MeshBufferID, MeshBuffer>::const_iterator it;
		it = meshBuffers.find(id);

		if ( it != meshBuffers.cend() )
			return it->second;

		std::cout << "Warning (ResourceManager::getBuffer): buffer with id " << id << " was not found." << std::endl;
		return MeshBuffer();
	}

	ResourceManager::MeshBufferID ResourceManager::addMeshBuffer( const std::vector<float> &vertices, const std::vector<UINT> &indices,
																const VertexInputLayout &inputDescriptor, HRESULT result )
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
		result = dxDevice->CreateBuffer( &buffDesc, &vertResource, &vertBuff );

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
		result = dxDevice->CreateBuffer( &buffDesc, &indexResource, &indexBuff );

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

		meshBuffers.insert( std::pair<MeshBufferID, MeshBuffer>(nextBufferID, mBuff) );
		nextBufferID++;

		result = S_OK;

		return nextBufferID - 1;
	}

	void ResourceManager::releaseBuffer( MeshBufferID id )
	{
		std::map<MeshBufferID, MeshBuffer>::iterator it;
		it = meshBuffers.find(id);

		if ( it != meshBuffers.end() )
		{
			if ( it->second.vertBuff )
				it->second.vertBuff->Release();
			if ( it->second.indexBuff )
				it->second.indexBuff->Release();

			meshBuffers.erase(id);
		}
	}



	Material* ResourceManager::getMaterial( MaterialID id ) const
	{
		std::map<MaterialID, Material*>::const_iterator it;
		it = materials.find(id);

		if ( it != materials.cend() )
			return it->second;

		std::cout << "Warning (ResourceManager::getMaterial): Material with id " << id << " was not found." << std::endl;
		return nullptr;
	}

	ResourceManager::MaterialID ResourceManager::claimMaterial( _In_ Material *mat )
	{
		materials.insert( std::pair<MaterialID, Material*>(nextMaterialID, mat) );
		nextMaterialID++;

		return nextMaterialID - 1;
	}

	void ResourceManager::releaseMaterial( MaterialID id )
	{
		std::map<MaterialID, Material*>::iterator it;
		it = materials.find(id);

		if ( it != materials.end() )
			delete it->second;
	}
}
