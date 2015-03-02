
#pragma once

#include <d3d11.h>
#include <map>
#include <vector>

#include "Defines.h"
#include "Material.h"


namespace kgx
{
	class ResourceManager
	{
		public:
			static ResourceManager* getInst();
			static void construct( _In_ ID3D11Device *dxDevice );
			static void destroy();


			typedef UINT MeshBufferID;
			MeshBuffer getBuffer( MeshBufferID id ) const;
			MeshBufferID addMeshBuffer( const std::vector<float> &vertices, const std::vector<UINT> &indices,
								const VertexInputLayout &inputDescriptor, HRESULT result );
			void releaseBuffer( MeshBufferID id );

			typedef UINT MaterialID;
			Material* getMaterial( MaterialID id ) const;
			MaterialID claimMaterial( _In_ Material *mat );
			void releaseMaterial( MaterialID id );


		private:
			ResourceManager( _In_ ID3D11Device *dxDevice );
			~ResourceManager();
			// no copying allowed
			ResourceManager( const ResourceManager& );
			ResourceManager& operator=( const ResourceManager& );

			static ResourceManager* inst;

			ID3D11Device *dxDevice;

			
			MeshBufferID nextBufferID;
			std::map<MeshBufferID, MeshBuffer> meshBuffers;
			MaterialID nextMaterialID;
			std::map<MaterialID, Material*> materials;
	};
}
