
#include <iostream>

#include "MovableObject.h"

namespace kgx
{
	MovableObject::MovableObject()
		: MovableObject("")
	{
	}

	MovableObject::MovableObject( const std::string &name )
		: Object(name), m_modelMatrix(), m_xPos(0.0f), m_yPos(0.0f), m_zPos(0.0f), m_xScale(1.0f), m_yScale(1.0f), m_zScale(1.0f)
	{
		// set m_modelMatrix to identity
		DirectX::XMStoreFloat4x4( &m_modelMatrix, DirectX::XMMatrixIdentity() );

		std::cout << "--- Model Matrix ---" << std::endl
			<< m_modelMatrix._11 << ", " << m_modelMatrix._12 << ", " << m_modelMatrix._13 << ", " << m_modelMatrix._14 << std::endl
			<< m_modelMatrix._21 << ", " << m_modelMatrix._22 << ", " << m_modelMatrix._23 << ", " << m_modelMatrix._24 << std::endl
			<< m_modelMatrix._31 << ", " << m_modelMatrix._32 << ", " << m_modelMatrix._33 << ", " << m_modelMatrix._34 << std::endl
			<< m_modelMatrix._41 << ", " << m_modelMatrix._42 << ", " << m_modelMatrix._43 << ", " << m_modelMatrix._44 << std::endl;
	}

	MovableObject::~MovableObject()
	{
	}


	DirectX::XMFLOAT4X4 MovableObject::getModelMatrix() const
	{
		return m_modelMatrix;
	}

	DirectX::XMFLOAT4X4 MovableObject::getNormalMatrix() const
	{
		//TODO: maybe cache this, if necessary?

		// the normal matrix is the inverse-transpose of the model matrix
		DirectX::XMMATRIX modelMat  = DirectX::XMLoadFloat4x4( &m_modelMatrix );
		DirectX::XMMATRIX normalMat = DirectX::XMMatrixInverse( nullptr, modelMat );
		normalMat = DirectX::XMMatrixTranspose( normalMat );

		DirectX::XMFLOAT4X4 result;
		DirectX::XMStoreFloat4x4( &result, normalMat );

		return result;
	}


	DirectX::XMFLOAT3 MovableObject::getPosition() const
	{
		return DirectX::XMFLOAT3( m_xPos, m_yPos, m_zPos );
	}

	DirectX::XMFLOAT3 MovableObject::getScale() const
	{
		return DirectX::XMFLOAT3( m_xScale, m_yScale, m_zScale );
	}


	void MovableObject::translate( float deltaX, float deltaY, float deltaZ )
	{
		m_xPos += deltaX;
		m_yPos += deltaY;
		m_zPos += deltaZ;

		DirectX::XMMATRIX translMat = DirectX::XMMatrixTranslation( deltaX, deltaY, deltaZ );
		DirectX::XMMATRIX modelMat  = DirectX::XMLoadFloat4x4( &m_modelMatrix );
		modelMat = DirectX::XMMatrixMultiply( modelMat, translMat );

		DirectX::XMStoreFloat4x4( &m_modelMatrix, modelMat );
	}
	void MovableObject::setPosition( float x, float y, float z )
	{
		m_xPos = x;
		m_yPos = y;
		m_zPos = z;
		translate( 0.0f, 0.0f, 0.0f );
	}

	void MovableObject::scale( float deltaScale )
	{
		scale( deltaScale, deltaScale, deltaScale );
	}

	void MovableObject::scale( float deltaX, float deltaY, float deltaZ )
	{
		m_xScale *= deltaX;
		m_yScale *= deltaY;
		m_zScale *= deltaZ;

		DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling( deltaX, deltaY, deltaZ );
		DirectX::XMMATRIX modelMat = DirectX::XMLoadFloat4x4( &m_modelMatrix );
		modelMat = DirectX::XMMatrixMultiply( modelMat, scaleMat );

		DirectX::XMStoreFloat4x4( &m_modelMatrix, modelMat );
	}

	void MovableObject::setScale( float newScale )
	{
		setScale( newScale, newScale, newScale );
	}

	void MovableObject::setScale( float xNewScale, float yNewScale, float zNewScale )
	{
		float xFactor = xNewScale / m_xScale;
		float yFactor = yNewScale / m_yScale;
		float zFactor = zNewScale / m_zScale;

		std::cout << "curScaleX: " << m_xScale << std::endl;
		std::cout << "xFactor: " << xFactor << std::endl;
		std::cout << "newScaleX: " << (m_xScale * xFactor) << std::endl;

		m_xScale *= xFactor;
		m_yScale *= yFactor;
		m_zScale *= zFactor;

		DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling( xFactor, yFactor, zFactor );
		DirectX::XMMATRIX modelMat = DirectX::XMLoadFloat4x4( &m_modelMatrix );
		modelMat = DirectX::XMMatrixMultiply( modelMat, scaleMat );

		DirectX::XMStoreFloat4x4( &m_modelMatrix, modelMat );
	}
}
