
#include <iostream>

#include "Material.h"
#include "RenderableObject.h"

namespace kgx
{
	RenderableObject::RenderableObject( _In_ ID3D11Device *dxDevice, MeshBuffer buff, const std::vector<ObjectContainer> &objectContainers,
										D3D11_PRIMITIVE_TOPOLOGY meshTopology )
		: m_dxDev( dxDevice ), m_dxDevCont( 0 ), m_meshBuff( buff ), m_matContainers( objectContainers ), m_topology( meshTopology )
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );

	};

	RenderableObject::~RenderableObject()
	{
		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}


	void RenderableObject::draw( _In_ Camera *renderCam )
	{
		// bind Vertex- and Index-buffers to IA Stage
		UINT bufferStride = m_meshBuff.inputDescriptor.getBufferStride();
		UINT offset       = 0;
		m_dxDevCont->IASetVertexBuffers( 0, 1, &m_meshBuff.vertBuff, &bufferStride, &offset );
		m_dxDevCont->IASetIndexBuffer( m_meshBuff.indexBuff, DXGI_FORMAT_R32_UINT, 0 );
		m_dxDevCont->IASetPrimitiveTopology( m_topology );

		std::vector<ObjectContainer>::iterator objIt;
		for ( objIt = m_matContainers.begin(); objIt != m_matContainers.end(); ++objIt )
		{
			// activate material
			objIt->mat->activate( renderCam, this );

			// draw Meshes
			std::vector<Mesh>::iterator meshIt;
			for ( meshIt = objIt->meshes.begin(); meshIt != objIt->meshes.end(); ++meshIt )
				m_dxDevCont->DrawIndexed( meshIt->indexCount, meshIt->startIndex, 0 );
		}
	}
}
