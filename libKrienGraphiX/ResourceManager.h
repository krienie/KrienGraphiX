
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
								const VertexInputLayout &inputDescriptor, HRESULT &result );
			void releaseBuffer( MeshBufferID id );

			Material* getMaterial( Material::MaterialID id ) const;
			Material* createMaterial();
			void releaseMaterial( Material::MaterialID id );


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
			Material::MaterialID m_nextMaterialID;
			std::map<Material::MaterialID, Material*> m_materials;
	};
}
