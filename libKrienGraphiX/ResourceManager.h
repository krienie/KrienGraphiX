
#pragma once

#include <d3d11.h>
#include <map>
#include <vector>

#include "Defines.h"
//#include "Material.h"


namespace kgx
{
	class Material;

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

			//TODO: probably create a function like createMaterial, so the ResourceManager creates the materials and doesn't claim them
			typedef UINT MaterialID;
			Material* getMaterial( MaterialID id ) const;
			MaterialID claimMaterial( _In_ Material *mat );
			void releaseMaterial( MaterialID id );


		private:
			explicit ResourceManager( _In_ ID3D11Device *dxDevice );
			~ResourceManager();
			// no copying allowed
			ResourceManager( const ResourceManager& );
			ResourceManager& operator=( const ResourceManager& );

			static ResourceManager* m_inst;

			ID3D11Device *m_dxDev;

			
			MeshBufferID m_nextBufferID;
			std::map<MeshBufferID, MeshBuffer> m_meshBuffers;
			MaterialID m_nextMaterialID;
			std::map<MaterialID, Material*> m_materials;
	};
}
