
#include <iostream>

#include "Scene.h"
#include "Camera.h"

namespace kgx
{
	Camera::Camera( const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up )
		//: Camera( DirectX::XM_PIDIV4, 1.0f, 0.001f, 500.0f, eye, target, up )
	{
		//TODO: fix this obvious error... => use delegating constructors
		//Camera::Camera( DirectX::XM_PIDIV4, 1.0f, 0.001f, 500.0f, eye, target, up );
	}


	Camera::Camera( float fovY, float aspect, float nearZ, float farZ,
						const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target_, const DirectX::XMFLOAT3 &up )
		: parentScene(nullptr), projMatrix(), viewMatrix(), position(eye), target(target_), camUp(up),
			fov(fovY), aspectRatio(aspect), nearZ(nearZ), farZ(farZ)
	{
		// create perspective matrix
		DirectX::XMMATRIX tempPersp = DirectX::XMMatrixPerspectiveFovLH( fovY, aspect, nearZ, farZ );
		DirectX::XMStoreFloat4x4( &projMatrix, tempPersp );

		std::cout << "--- Perspective Matrix ---" << std::endl
			<< projMatrix._11 << ", " << projMatrix._12 << ", " << projMatrix._13 << ", " << projMatrix._14 << std::endl
			<< projMatrix._21 << ", " << projMatrix._22 << ", " << projMatrix._23 << ", " << projMatrix._24 << std::endl
			<< projMatrix._31 << ", " << projMatrix._32 << ", " << projMatrix._33 << ", " << projMatrix._34 << std::endl
			<< projMatrix._41 << ", " << projMatrix._42 << ", " << projMatrix._43 << ", " << projMatrix._44 << std::endl;

		
		// set camera mPosition and direction
		lookAt( eye, target, up );

		std::cout << "--- View Matrix ---" << std::endl
			<< viewMatrix._11 << ", " << viewMatrix._12 << ", " << viewMatrix._13 << ", " << viewMatrix._14 << std::endl
			<< viewMatrix._21 << ", " << viewMatrix._22 << ", " << viewMatrix._23 << ", " << viewMatrix._24 << std::endl
			<< viewMatrix._31 << ", " << viewMatrix._32 << ", " << viewMatrix._33 << ", " << viewMatrix._34 << std::endl
			<< viewMatrix._41 << ", " << viewMatrix._42 << ", " << viewMatrix._43 << ", " << viewMatrix._44 << std::endl;
	}


	Camera::Camera( const Camera &other )
		: parentScene(other.parentScene), projMatrix(other.projMatrix), viewMatrix(other.viewMatrix), position(other.position),
		target(other.target), camUp(other.camUp), fov(other.fov), aspectRatio(other.aspectRatio), nearZ(other.nearZ), farZ(other.farZ)
	{

	}

	/*Camera::~Camera()
	{
	}*/

	Camera& Camera::operator=( const Camera &rhs )
	{
		parentScene = rhs.parentScene;
		projMatrix  = rhs.projMatrix;
		viewMatrix  = rhs.viewMatrix;
		position    = rhs.position;
		target      = rhs.target;
		camUp       = rhs.camUp;
		fov         = rhs.fov;
		aspectRatio = rhs.aspectRatio;
		nearZ       = rhs.nearZ;
		farZ        = rhs.farZ;

		return *this;
	}


	DirectX::XMFLOAT4X4 Camera::getProjMatrix() const
	{
		return projMatrix;
	}

	DirectX::XMFLOAT4X4 Camera::getViewMatrix() const
	{
		return viewMatrix;
	}

	DirectX::XMFLOAT3 Camera::getPosition() const
	{
		return position;
	}

	DirectX::XMFLOAT3 Camera::getTarget() const
	{
		return target;
	}

	float Camera::getFOV() const
	{
		return fov;
	}

	float Camera::getAspectRatio() const
	{
		return aspectRatio;
	}

	float Camera::getNearZ() const
	{
		return nearZ;
	}

	float Camera::getFarZ() const
	{
		return farZ;
	}


	void Camera::setParentScene( _In_ Scene *scene )
	{
		parentScene = scene;
	}

	/**
	 * Renders current camera view. Assumes a viewport has already been assigned to the DirectX pipeline
	 */
	void Camera::renderCurrentView()
	{
		if ( parentScene )
			parentScene->render(this);
	}


	void Camera::lookAt( const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target_, const DirectX::XMFLOAT3 &up )
	{
		// create camera view matrix
		DirectX::XMVECTORF32 xmEye    = { eye.x, eye.y, eye.z, 0.0f };
		DirectX::XMVECTORF32 xmTarget = { target_.x, target_.y, target_.z, 0.0f };
		DirectX::XMVECTORF32 xmUp     = { up.x, up.y, up.z, 0.0f };
		DirectX::XMMATRIX tempView    =  DirectX::XMMatrixLookAtLH(xmEye, xmTarget, xmUp);
		DirectX::XMStoreFloat4x4( &viewMatrix, tempView );

		// save eye-, target- and up-vectors
		this->position = eye;
		this->target   = target_;
		this->camUp    = up;
	}

	/*void Camera::moveForward( float dist )
	{
		DirectX::XMVECTOR eyeVect    = DirectX::XMLoadFloat3( &mPosition );
		DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3( &mTarget );
		DirectX::XMVECTOR dirVect = DirectX::XMVector3Normalize( DirectX::XMVectorSubtract(targetVect, eyeVect) );
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
		DirectX::XMVECTOR eyeVect    = DirectX::XMLoadFloat3( &mPosition );
		DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3( &mTarget );
		DirectX::XMVECTOR dirVect    = DirectX::XMVectorSubtract(targetVect, eyeVect);

		DirectX::XMVECTOR upVect   = DirectX::XMLoadFloat3( &mCamUp );
		DirectX::XMVECTOR rightVect = DirectX::XMVector3Normalize( DirectX::XMVector3Cross( dirVect, upVect ) );
		rightVect = DirectX::XMVectorScale(rightVect, dist);

		DirectX::XMFLOAT3 rightFloat3;
		DirectX::XMStoreFloat3( &rightFloat3, rightVect );

		translate( rightFloat3 );
	}

	void Camera::translate( const DirectX::XMFLOAT3 &deltaPos )
	{

		DirectX::XMFLOAT3 newPos = DirectX::XMFLOAT3( mPosition.x + deltaPos.x,
													  mPosition.y + deltaPos.y,
													  mPosition.z + deltaPos.z );
		DirectX::XMFLOAT3 newTarget = DirectX::XMFLOAT3( mTarget.x + deltaPos.x,
													     mTarget.y + deltaPos.y,
													     mTarget.z + deltaPos.z );
		lookAt( newPos, newTarget, mCamUp );
	}

	void Camera::rotateUp( float degrees )
	{
		DirectX::XMVECTOR upVect     = DirectX::XMLoadFloat3( &mCamUp );
		DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3( &mTarget );
		DirectX::XMVECTOR eyeVect    =  DirectX::XMLoadFloat3( &mPosition );
		DirectX::XMVECTOR dirVect    = DirectX::XMVectorSubtract(targetVect, eyeVect);

		DirectX::XMVECTOR rotAxis = DirectX::XMVector3Cross( dirVect, upVect );
		DirectX::XMMATRIX rotMat  = DirectX::XMMatrixRotationAxis( rotAxis, DirectX::XMConvertToRadians(degrees) );

		DirectX::XMVECTOR newTarget = DirectX::XMVectorAdd( DirectX::XMVector3Transform( dirVect, rotMat ), eyeVect );
		DirectX::XMStoreFloat3( &mTarget, newTarget );


		lookAt( mPosition, mTarget, mCamUp );
	}

	void Camera::rotateDown( float degrees )
	{
		rotateUp(-degrees);
	}

	void Camera::rotateLeft( float degrees )
	{
		DirectX::XMVECTOR upVect     = DirectX::XMLoadFloat3( &mCamUp );
		DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3( &mTarget );
		DirectX::XMVECTOR eyeVect    =  DirectX::XMLoadFloat3( &mPosition );
		DirectX::XMVECTOR dirVect    = DirectX::XMVector3Normalize( DirectX::XMVectorSubtract(targetVect, eyeVect) );

		DirectX::XMVECTOR rightVect = DirectX::XMVector3Cross( dirVect, upVect );
		DirectX::XMVECTOR rotAxis   = DirectX::XMVector3Cross( rightVect, dirVect );
		DirectX::XMMATRIX rotMat    = DirectX::XMMatrixRotationAxis( rotAxis, DirectX::XMConvertToRadians(degrees) );

		DirectX::XMVECTOR newTarget = DirectX::XMVectorAdd( DirectX::XMVector3Transform( dirVect, rotMat ), eyeVect );
		DirectX::XMStoreFloat3( &mTarget, newTarget );


		lookAt( mPosition, mTarget, mCamUp );
	}

	void Camera::rotateRight( float degrees )
	{
		rotateLeft(-degrees);
	}*/
}
