
#include <iostream>

#include "ShaderProgram.h"
#include "RenderableObject.h"

namespace kgx
{
	RenderableObject::RenderableObject( ID3D11Device *dxDevice, MeshBuffer buff, const std::vector<MaterialMeshContainer> &matMeshContainers,
										D3D11_PRIMITIVE_TOPOLOGY meshTopology )
		: RenderableObject(dxDevice, buff, matMeshContainers, meshTopology, "")
	{
	}
	RenderableObject::RenderableObject( ID3D11Device *dxDevice, MeshBuffer buff, const std::vector<MaterialMeshContainer> &matMeshContainers,
					  D3D11_PRIMITIVE_TOPOLOGY meshTopology, const std::string &name )
		: MovableObject(name), m_dxDev(dxDevice), m_dxDevCont(0), m_meshBuff(buff), m_matMeshContainers(matMeshContainers),
			m_topology(meshTopology), m_originalFilename()
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );
	}

	RenderableObject::~RenderableObject()
	{
		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}

	std::string RenderableObject::getOriginalFilename() const
	{
		return m_originalFilename;
	}
	void RenderableObject::setOriginalFilename( const std::string &filename )
	{
		m_originalFilename = filename;
	}

	void RenderableObject::draw( Camera *renderCam, ShaderProgram *shaderProg )
	{
		// bind Vertex- and Index-buffers to IA Stage
		UINT bufferStride = m_meshBuff.inputDescriptor.getBufferStride();
		UINT offset       = 0;
		m_dxDevCont->IASetVertexBuffers( 0, 1, &m_meshBuff.vertBuff, &bufferStride, &offset );
		m_dxDevCont->IASetIndexBuffer( m_meshBuff.indexBuff, DXGI_FORMAT_R32_UINT, 0 );
		m_dxDevCont->IASetPrimitiveTopology( m_topology );

		// update ShaderProgram constants
		shaderProg->updateShaderVar( ShaderProgram::ShaderType::Vertex, "kgx_modelMatrix", &(m_modelMatrix.m[0]) );
		shaderProg->updateShaderVar( ShaderProgram::ShaderType::Vertex, "kgx_normalMatrix", &(getNormalMatrix().m[0]) );

		std::vector<MaterialMeshContainer>::iterator matMeshIt;
		for ( matMeshIt = m_matMeshContainers.begin(); matMeshIt != m_matMeshContainers.end(); ++matMeshIt )
		{
			// update shader material info
			shaderProg->updateShaderVar( ShaderProgram::ShaderType::Pixel, "kgx_diffuse", &matMeshIt->material.diffuse );
			shaderProg->updateShaderVar( ShaderProgram::ShaderType::Pixel, "kgx_specular", &matMeshIt->material.specular );

			shaderProg->commitAllChanges();

			// draw Meshes
			std::vector<Mesh>::iterator meshIt;
			for ( meshIt = matMeshIt->meshes.begin(); meshIt != matMeshIt->meshes.end(); ++meshIt )
			{
				//TODO: if the meshes are sorted in the vertex buffer, they can be drawn all at once => sort meshes in the vertex buffer
				m_dxDevCont->DrawIndexed( meshIt->indexCount, meshIt->startIndex, 0 );
			}
		}
	}
}
