
#include <iostream>

#include "Material.h"
#include "RenderableObject.h"

namespace kgx
{
	RenderableObject::RenderableObject( _In_ ID3D11Device *dxDevice, MeshBuffer buff, const std::vector<ObjectContainer> &materialContainers,
										D3D11_PRIMITIVE_TOPOLOGY meshTopology )
		: m_dxDev(dxDevice), m_dxDevCont(0), m_meshBuff(buff), m_matContainers(materialContainers), m_topology(meshTopology), m_modelMatrix()
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );

		// set m_modelMatrix to identity
		DirectX::XMStoreFloat4x4( &m_modelMatrix, DirectX::XMMatrixIdentity() );

		std::cout << "--- Model Matrix ---" << std::endl
			<< m_modelMatrix._11 << ", " << m_modelMatrix._12 << ", " << m_modelMatrix._13 << ", " << m_modelMatrix._14 << std::endl
			<< m_modelMatrix._21 << ", " << m_modelMatrix._22 << ", " << m_modelMatrix._23 << ", " << m_modelMatrix._24 << std::endl
			<< m_modelMatrix._31 << ", " << m_modelMatrix._32 << ", " << m_modelMatrix._33 << ", " << m_modelMatrix._34 << std::endl
			<< m_modelMatrix._41 << ", " << m_modelMatrix._42 << ", " << m_modelMatrix._43 << ", " << m_modelMatrix._44 << std::endl;
	};

	RenderableObject::~RenderableObject()
	{
		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}


	DirectX::XMFLOAT4X4 RenderableObject::getModelMatrix() const
	{
		return m_modelMatrix;
	}


	void RenderableObject::draw( _In_ Camera *renderCam )
	{
		// bind Vertex- and Index-buffers to IA Stage
		UINT bufferStride = m_meshBuff.inputDescriptor.getBufferStride();
		UINT offset       = 0;
		m_dxDevCont->IASetVertexBuffers( 0, 1, &m_meshBuff.vertBuff, &bufferStride, &offset );
		m_dxDevCont->IASetIndexBuffer( m_meshBuff.indexBuff, DXGI_FORMAT_R32_UINT, 0 );

		std::vector<ObjectContainer>::iterator objIt;
		for ( objIt = m_matContainers.begin(); objIt != m_matContainers.end(); ++objIt )
		{
			// activate material
			objIt->mat->activate( renderCam, this );

			// draw Meshes
			std::vector<Mesh>::iterator meshIt;
			for ( meshIt = objIt->meshes.begin(); meshIt != objIt->meshes.end(); ++meshIt )
			{
				m_dxDevCont->IASetPrimitiveTopology( m_topology );
				m_dxDevCont->DrawIndexed( meshIt->indexCount, meshIt->startIndex, 0 );
			}
		}
	}
}
