
#pragma once

#include <d3d11.h>
#include <map>
#include <vector>

#include "Defines.h"
#include "ShaderProgram.h"


namespace kgx
{
	class ResourceManager
	{
		public:
			static ResourceManager* getInst();
			static void construct( ID3D11Device *dxDevice );
			static void destroy();

			MeshBuffer getMeshBuffer( MeshBufferID id ) const;
			MeshBufferID addMeshBuffer( const std::vector<float> &vertices, const std::vector<UINT> &indices,
								const VertexInputLayout &inputDescriptor, HRESULT &result );
			void releaseBuffer( MeshBufferID id );

			Material getMaterial( const std::string &matName );
			ShaderProgram::ShaderProgramID getDefaultShaderProgram();

			ShaderProgram* getShaderProgram( ShaderProgram::ShaderProgramID id ) const;
			ShaderProgram* createShaderProgram();
			void releaseShaderProgram( ShaderProgram::ShaderProgramID id );

			void clearResources();

		private:
			explicit ResourceManager( ID3D11Device *dxDevice );
			~ResourceManager();
			// no copying allowed
			ResourceManager( const ResourceManager& );
			ResourceManager& operator=( const ResourceManager& );

			bool loadMaterials();

			static ResourceManager* m_inst;

			ID3D11Device *m_dxDev;
			
			MeshBufferID m_nextBufferID;
			std::map<MeshBufferID, MeshBuffer> m_meshBuffers;
			std::map<std::string, Material> m_materials;
			ShaderProgram::ShaderProgramID m_nextShaderProgramID;
			ShaderProgram::ShaderProgramID m_defaultShaderProgram;
			std::map<ShaderProgram::ShaderProgramID, ShaderProgram*> m_shaderPrograms;
	};
}
