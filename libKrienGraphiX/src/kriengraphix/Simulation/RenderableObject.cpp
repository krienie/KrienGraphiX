
#include "Simulation/RenderableObject.h"

#include "Rendering/MeshBuffer.h"

namespace kgx
{
	RenderableObject::RenderableObject( const std::string &name, D3D11_PRIMITIVE_TOPOLOGY topology, MeshBuffer *meshBuffer,
										UINT indexCount, UINT startIndex, UINT baseVertex )
		: m_isDirty(true), m_name(name), m_topology(topology), m_meshBuffer(meshBuffer),
			m_indexCount(indexCount), m_startIndex(startIndex), m_baseVertex(baseVertex),
			m_material(), m_xPos(0.0f), m_yPos(0.0f), m_zPos(0.0f),
			m_xScale(1.0f), m_yScale(1.0f), m_zScale(1.0f),
			m_modelMatrix(), m_normalMatrix()
	{
	}

	RenderableObject::~RenderableObject()
	{
		if ( m_meshBuffer )
			delete m_meshBuffer;
	}

	void RenderableObject::setPosition( float xPos, float yPos, float zPos )
	{
		m_xPos = xPos;
		m_yPos = yPos;
		m_zPos = zPos;

		m_isDirty = true;
	}

	void RenderableObject::setScale( float xScale, float yScale, float zScale )
	{
		m_xScale = xScale;
		m_yScale = yScale;
		m_zScale = zScale;

		m_isDirty = true;
	}

	std::string RenderableObject::getName() const
	{
		return m_name;
	}

	D3D11_PRIMITIVE_TOPOLOGY RenderableObject::getTopology() const
	{
		return m_topology;
	}

	const MeshBuffer* RenderableObject::getMeshBuffer() const
	{
		return m_meshBuffer;
	}

	UINT RenderableObject::getIndexCount() const
	{
		return m_indexCount;
	}

	UINT RenderableObject::getStartIndex() const
	{
		return m_startIndex;
	}

	UINT RenderableObject::getBaseVertex() const
	{
		return m_baseVertex;
	}

	void RenderableObject::setMaterial( const Material & material )
	{
		m_material = material;
	}
	Material RenderableObject::getMaterial() const
	{
		return m_material;
	}

	DirectX::XMFLOAT4X4 RenderableObject::getModelMatrix()
	{
		if ( m_isDirty )
		{
			DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation( m_xPos, m_yPos, m_zPos );
			DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling( m_xScale, m_yScale, m_zScale );
			DirectX::XMMATRIX modelMat = DirectX::XMMatrixMultiply( scaleMat, transMat );

			DirectX::XMStoreFloat4x4( &m_modelMatrix, modelMat );

			m_isDirty = false;
		}

		return m_modelMatrix;
	}

	DirectX::XMFLOAT4X4 RenderableObject::getNormalMatrix()
	{
		if ( m_isDirty )
		{
			DirectX::XMMATRIX modelMat  = DirectX::XMLoadFloat4x4( &m_modelMatrix );
			DirectX::XMMATRIX normalMat = DirectX::XMMatrixInverse( nullptr, modelMat );
			normalMat = DirectX::XMMatrixTranspose( normalMat );

			DirectX::XMStoreFloat4x4( &m_normalMatrix, normalMat );

			m_isDirty = false;
		}

		return m_normalMatrix;
	}
}
