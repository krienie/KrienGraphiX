
#include <iostream>

#include "RenderableObject.h"

namespace kgx
{
	RenderableObject::RenderableObject( _In_ ID3D11Device *dxDevice, MeshBuffer buff, const std::vector<ObjectContainer> &materialContainers,
										D3D11_PRIMITIVE_TOPOLOGY meshTopology )
		: dxDev(dxDevice), dxDevCont(0), meshBuff(buff),  matContainers(materialContainers), topology(meshTopology), modelMatrix()
	{
		dxDev->GetImmediateContext( &dxDevCont );

		// set modelMatrix to identity
		DirectX::XMStoreFloat4x4( &modelMatrix, DirectX::XMMatrixIdentity() );

		std::cout << "--- Model Matrix ---" << std::endl
			<< modelMatrix._11 << ", " << modelMatrix._12 << ", " << modelMatrix._13 << ", " << modelMatrix._14 << std::endl
			<< modelMatrix._21 << ", " << modelMatrix._22 << ", " << modelMatrix._23 << ", " << modelMatrix._24 << std::endl
			<< modelMatrix._31 << ", " << modelMatrix._32 << ", " << modelMatrix._33 << ", " << modelMatrix._34 << std::endl
			<< modelMatrix._41 << ", " << modelMatrix._42 << ", " << modelMatrix._43 << ", " << modelMatrix._44 << std::endl;
	};

	RenderableObject::~RenderableObject()
	{
		if ( dxDevCont )
			dxDevCont->Release();
	}


	DirectX::XMFLOAT4X4 RenderableObject::getModelMatrix() const
	{
		return modelMatrix;
	}


	void RenderableObject::draw( _In_ Camera *renderCam )
	{
		// bind Vertex- and Index-buffers to IA Stage
		UINT bufferStride = meshBuff.inputDescriptor.getBufferStride();
		UINT offset       = 0;
		dxDevCont->IASetVertexBuffers( 0, 1, &meshBuff.vertBuff, &bufferStride, &offset );
		dxDevCont->IASetIndexBuffer( meshBuff.indexBuff, DXGI_FORMAT_R32_UINT, 0 );

		std::vector<ObjectContainer>::iterator objIt;
		for ( objIt = matContainers.begin(); objIt != matContainers.end(); ++objIt )
		{
			// activate material
			objIt->mat->activate( renderCam, this );

			// draw Meshes
			std::vector<Mesh>::iterator meshIt;
			for ( meshIt = objIt->meshes.begin(); meshIt != objIt->meshes.end(); ++meshIt )
			{
				dxDevCont->IASetPrimitiveTopology( topology );
				dxDevCont->DrawIndexed( meshIt->indexCount, meshIt->startIndex, 0 );
			}
		}
	}
}
