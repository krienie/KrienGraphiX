
#include <iostream>

#include "Scene.h"
#include "Camera.h"

namespace kgx
{
	Camera::Camera( const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &m_target, const DirectX::XMFLOAT3 &up )
		: Camera( DirectX::XM_PIDIV4, 1.0f, 0.001f, 500.0f, eye, m_target, up )
	{
	}


	Camera::Camera( float fovY, float aspect, float m_nearZ, float m_farZ,
						const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target_, const DirectX::XMFLOAT3 &up )
		: m_parentScene(nullptr), m_projMatrix(), m_viewMatrix(), m_position(eye), m_target(target_), m_camUp(up),
			m_fov(fovY), m_aspectRatio(aspect), m_nearZ(m_nearZ), m_farZ(m_farZ)
	{
		// create perspective matrix
		DirectX::XMMATRIX tempPersp = DirectX::XMMatrixPerspectiveFovRH( fovY, aspect, m_nearZ, m_farZ );
		DirectX::XMStoreFloat4x4( &m_projMatrix, tempPersp );

		std::cout << "--- Perspective Matrix ---" << std::endl
			<< m_projMatrix._11 << ", " << m_projMatrix._12 << ", " << m_projMatrix._13 << ", " << m_projMatrix._14 << std::endl
			<< m_projMatrix._21 << ", " << m_projMatrix._22 << ", " << m_projMatrix._23 << ", " << m_projMatrix._24 << std::endl
			<< m_projMatrix._31 << ", " << m_projMatrix._32 << ", " << m_projMatrix._33 << ", " << m_projMatrix._34 << std::endl
			<< m_projMatrix._41 << ", " << m_projMatrix._42 << ", " << m_projMatrix._43 << ", " << m_projMatrix._44 << std::endl;

		
		// set camera mPosition and direction
		lookAt( eye, m_target, up );

		std::cout << "--- View Matrix ---" << std::endl
			<< m_viewMatrix._11 << ", " << m_viewMatrix._12 << ", " << m_viewMatrix._13 << ", " << m_viewMatrix._14 << std::endl
			<< m_viewMatrix._21 << ", " << m_viewMatrix._22 << ", " << m_viewMatrix._23 << ", " << m_viewMatrix._24 << std::endl
			<< m_viewMatrix._31 << ", " << m_viewMatrix._32 << ", " << m_viewMatrix._33 << ", " << m_viewMatrix._34 << std::endl
			<< m_viewMatrix._41 << ", " << m_viewMatrix._42 << ", " << m_viewMatrix._43 << ", " << m_viewMatrix._44 << std::endl;
	}


	Camera::Camera( const Camera &other )
		: m_parentScene(other.m_parentScene), m_projMatrix(other.m_projMatrix), m_viewMatrix(other.m_viewMatrix), m_position(other.m_position),
		m_target(other.m_target), m_camUp(other.m_camUp), m_fov(other.m_fov), m_aspectRatio(other.m_aspectRatio), m_nearZ(other.m_nearZ), m_farZ(other.m_farZ)
	{
	}

	/*Camera::~Camera()
	{
	}*/

	Camera& Camera::operator=( const Camera &rhs )
	{
		if ( this != &rhs )
		{
			m_parentScene = rhs.m_parentScene;
			m_projMatrix  = rhs.m_projMatrix;
			m_viewMatrix  = rhs.m_viewMatrix;
			m_position    = rhs.m_position;
			m_target      = rhs.m_target;
			m_camUp       = rhs.m_camUp;
			m_fov         = rhs.m_fov;
			m_aspectRatio = rhs.m_aspectRatio;
			m_nearZ       = rhs.m_nearZ;
			m_farZ        = rhs.m_farZ;
		}

		return *this;
	}


	const DirectX::XMFLOAT4X4& Camera::getProjMatrix() const
	{
		return m_projMatrix;
	}

	const DirectX::XMFLOAT4X4& Camera::getViewMatrix() const
	{
		return m_viewMatrix;
	}

	const DirectX::XMFLOAT3& Camera::getPosition() const
	{
		return m_position;
	}

	const DirectX::XMFLOAT3& Camera::getTarget() const
	{
		return m_target;
	}

	float Camera::getFOV() const
	{
		return m_fov;
	}

	float Camera::getAspectRatio() const
	{
		return m_aspectRatio;
	}

	float Camera::getNearZ() const
	{
		return m_nearZ;
	}

	float Camera::getFarZ() const
	{
		return m_farZ;
	}


	void Camera::setParentScene( _In_ Scene *scene )
	{
		m_parentScene = scene;
	}

	/**
	 * Renders current camera view. Assumes a viewport has already been assigned to the DirectX pipeline
	 */
	void Camera::renderCurrentView()
	{
		if ( m_parentScene )
			m_parentScene->render(this);
	}


	void Camera::lookAt( const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target_, const DirectX::XMFLOAT3 &up )
	{
		// create camera view matrix
		DirectX::XMVECTORF32 xmEye    = { eye.x, eye.y, eye.z, 0.0f };
		DirectX::XMVECTORF32 xmTarget = { target_.x, target_.y, target_.z, 0.0f };
		DirectX::XMVECTORF32 xmUp     = { up.x, up.y, up.z, 0.0f };
		DirectX::XMMATRIX tempView    =  DirectX::XMMatrixLookAtRH(xmEye, xmTarget, xmUp);
		DirectX::XMStoreFloat4x4( &m_viewMatrix, tempView );

		// save eye-, m_target- and up-vectors
		this->m_position = eye;
		this->m_target   = target_;
		this->m_camUp    = up;
	}

	void Camera::moveForward( float dist )
	{
		DirectX::XMVECTOR eyeVect    = DirectX::XMLoadFloat3( &m_position );
		DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3( &m_target );
		DirectX::XMVECTOR dirVect    = DirectX::XMVector3Normalize( DirectX::XMVectorSubtract(targetVect, eyeVect) );
		dirVect = DirectX::XMVectorScale(dirVect, dist);

		DirectX::XMFLOAT3 dirFloat3;
		DirectX::XMStoreFloat3( &dirFloat3, dirVect );

		translate( dirFloat3 );
	}

	void Camera::moveBackward( float dist )
	{
		moveForward( -dist );
	}

	void Camera::moveLeft( float dist )
	{
		moveRight( -dist );
	}

	void Camera::moveRight( float dist )
	{
		DirectX::XMVECTOR eyeVect = DirectX::XMLoadFloat3( &m_position );
		DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3( &m_target );
		DirectX::XMVECTOR dirVect = DirectX::XMVectorSubtract( targetVect, eyeVect );

		DirectX::XMVECTOR upVect    = DirectX::XMLoadFloat3( &m_camUp );
		DirectX::XMVECTOR rightVect = DirectX::XMVector3Normalize( DirectX::XMVector3Cross( dirVect, upVect ) );
		rightVect = DirectX::XMVectorScale( rightVect, dist );

		DirectX::XMFLOAT3 rightFloat3;
		DirectX::XMStoreFloat3( &rightFloat3, rightVect );

		translate( rightFloat3 );
	}

	void Camera::translate( const DirectX::XMFLOAT3 &deltaPos )
	{

		DirectX::XMFLOAT3 newPos = DirectX::XMFLOAT3( m_position.x + deltaPos.x,
													  m_position.y + deltaPos.y,
													  m_position.z + deltaPos.z );
		DirectX::XMFLOAT3 newTarget = DirectX::XMFLOAT3( m_target.x + deltaPos.x,
														 m_target.y + deltaPos.y,
														 m_target.z + deltaPos.z );
		lookAt( newPos, newTarget, m_camUp );
	}

	void Camera::rotateUp( float degrees )
	{
		DirectX::XMVECTOR upVect     = DirectX::XMLoadFloat3( &m_camUp );
		DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3( &m_target );
		DirectX::XMVECTOR eyeVect    = DirectX::XMLoadFloat3( &m_position );
		DirectX::XMVECTOR dirVect    = DirectX::XMVectorSubtract(targetVect, eyeVect);

		DirectX::XMVECTOR rotAxis = DirectX::XMVector3Cross( dirVect, upVect );
		DirectX::XMMATRIX rotMat  = DirectX::XMMatrixRotationAxis( rotAxis, DirectX::XMConvertToRadians(degrees) );

		DirectX::XMVECTOR newTarget = DirectX::XMVectorAdd( DirectX::XMVector3Transform( dirVect, rotMat ), eyeVect );
		DirectX::XMStoreFloat3( &m_target, newTarget );


		lookAt( m_position, m_target, m_camUp );
	}

	void Camera::rotateDown( float degrees )
	{
		rotateUp(-degrees);
	}

	void Camera::rotateLeft( float degrees )
	{
		DirectX::XMVECTOR upVect     = DirectX::XMLoadFloat3( &m_camUp );
		DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3( &m_target );
		DirectX::XMVECTOR eyeVect    = DirectX::XMLoadFloat3( &m_position );
		DirectX::XMVECTOR dirVect    = DirectX::XMVector3Normalize( DirectX::XMVectorSubtract(targetVect, eyeVect) );

		DirectX::XMVECTOR rightVect = DirectX::XMVector3Cross( dirVect, upVect );
		DirectX::XMVECTOR rotAxis   = DirectX::XMVector3Cross( rightVect, dirVect );
		DirectX::XMMATRIX rotMat    = DirectX::XMMatrixRotationAxis( rotAxis, DirectX::XMConvertToRadians(degrees) );

		DirectX::XMVECTOR newTarget = DirectX::XMVectorAdd( DirectX::XMVector3Transform( dirVect, rotMat ), eyeVect );
		DirectX::XMStoreFloat3( &m_target, newTarget );


		lookAt( m_position, m_target, m_camUp );
	}

	void Camera::rotateRight( float degrees )
	{
		rotateLeft(-degrees);
	}
}
