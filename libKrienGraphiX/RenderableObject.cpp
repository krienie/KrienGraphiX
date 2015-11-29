
#include <iostream>

#include "ShaderProgram.h"
#include "RenderableObject.h"

namespace kgx
{
	RenderableObject::RenderableObject( ID3D11Device *dxDevice, MeshBuffer buff, const std::vector<ObjectContainer> &objectContainers,
										D3D11_PRIMITIVE_TOPOLOGY meshTopology )
		: RenderableObject(dxDevice, buff, objectContainers, meshTopology, "")
	{
	}
	RenderableObject::RenderableObject( ID3D11Device *dxDevice, MeshBuffer buff, const std::vector<ObjectContainer> &objectContainers,
					  D3D11_PRIMITIVE_TOPOLOGY meshTopology, const std::string &name )
		: MovableObject(name), m_dxDev(dxDevice), m_dxDevCont(0), m_meshBuff(buff), m_objContainers(objectContainers), m_topology(meshTopology)
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );
	}

	RenderableObject::~RenderableObject()
	{
		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}

	void RenderableObject::draw( Camera *renderCam, const std::vector<Light> &lights, const DirectX::XMFLOAT4 &ambientColor )
	{
		// bind Vertex- and Index-buffers to IA Stage
		UINT bufferStride = m_meshBuff.inputDescriptor.getBufferStride();
		UINT offset       = 0;
		m_dxDevCont->IASetVertexBuffers( 0, 1, &m_meshBuff.vertBuff, &bufferStride, &offset );
		m_dxDevCont->IASetIndexBuffer( m_meshBuff.indexBuff, DXGI_FORMAT_R32_UINT, 0 );
		m_dxDevCont->IASetPrimitiveTopology( m_topology );

		std::vector<ObjectContainer>::iterator objIt;
		for ( objIt = m_objContainers.begin(); objIt != m_objContainers.end(); ++objIt )
		{
			// activate ShaderProgram
			objIt->shaderProg->activate( renderCam, this );
			objIt->shaderProg->updateShaderVar( ShaderProgram::ShaderType::Pixel, "lights", lights.data() );
			objIt->shaderProg->updateShaderVar( ShaderProgram::ShaderType::Pixel, "ambLightClr", &ambientColor );

			// draw Meshes
			//TODO: if the meshes are sorted in the vertex buffer, they can be drawn all at once => sort meshes in the vertex buffer according to shaderprogram
			std::vector<Mesh>::iterator meshIt;
			for ( meshIt = objIt->meshes.begin(); meshIt != objIt->meshes.end(); ++meshIt )
				m_dxDevCont->DrawIndexed( meshIt->indexCount, meshIt->startIndex, 0 );
		}
	}
}
