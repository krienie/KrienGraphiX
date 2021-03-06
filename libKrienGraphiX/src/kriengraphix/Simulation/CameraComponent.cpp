
#include "Simulation/CameraComponent.h"

#include "Simulation/Scene.h"

namespace kgx
{
    CameraComponent::CameraComponent(SceneObject *owner, const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &m_target, const DirectX::XMFLOAT3 &up)
        : CameraComponent(owner, DirectX::XM_PIDIV4, 1.0f, 0.001f, 5000.0f, eye, m_target, up)
    {
    }

    CameraComponent::CameraComponent(SceneObject *owner, float fovY, float aspect, float m_nearZ, float m_farZ,
                    const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up)
        : SceneObjectComponent(owner), m_projMatrix(), m_viewMatrix(), m_eye(eye), m_target(target), m_camUp(up),
        m_fov(fovY), m_aspectRatio(aspect), m_nearZ(m_nearZ), m_farZ(m_farZ)
    {
        // create perspective matrix
        DirectX::XMMATRIX tempPersp = DirectX::XMMatrixPerspectiveFovRH( fovY, aspect, m_nearZ, m_farZ );
        DirectX::XMStoreFloat4x4( &m_projMatrix, tempPersp );

        // set camera mPosition and direction
        lookAt( eye, m_target, up );
    }

    const DirectX::XMFLOAT4X4& CameraComponent::getProjMatrix() const
    {
        return m_projMatrix;
    }

    const DirectX::XMFLOAT4X4& CameraComponent::getViewMatrix() const
    {
        return m_viewMatrix;
    }

    const DirectX::XMFLOAT3& CameraComponent::getEye() const
    {
        return m_eye;
    }

    const DirectX::XMFLOAT3& CameraComponent::getTarget() const
    {
        return m_target;
    }

    const DirectX::XMFLOAT3& CameraComponent::getUp() const
    {
        return m_camUp;
    }

    float CameraComponent::getFOV() const
    {
        return m_fov;
    }

    float CameraComponent::getAspectRatio() const
    {
        return m_aspectRatio;
    }

    float CameraComponent::getNearZ() const
    {
        return m_nearZ;
    }

    float CameraComponent::getFarZ() const
    {
        return m_farZ;
    }

    void CameraComponent::lookAt( const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target_, const DirectX::XMFLOAT3 &up )
    {
        // create CameraComponent view matrix
        DirectX::XMVECTORF32 xmEye    = { eye.x, eye.y, eye.z, 0.0f };
        DirectX::XMVECTORF32 xmTarget = { target_.x, target_.y, target_.z, 0.0f };
        DirectX::XMVECTORF32 xmUp     = { up.x, up.y, up.z, 0.0f };
        DirectX::XMMATRIX tempView    = DirectX::XMMatrixLookAtRH( xmEye, xmTarget, xmUp );
        DirectX::XMStoreFloat4x4( &m_viewMatrix, tempView );

        // save eye-, m_target- and up-vectors
        this->m_eye = eye;
        this->m_target = target_;
        this->m_camUp = up;
    }

    void CameraComponent::moveForward( float dist )
    {
        DirectX::XMVECTOR eyeVect    = DirectX::XMLoadFloat3( &m_eye );
        DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3( &m_target );
        DirectX::XMVECTOR dirVect    = DirectX::XMVector3Normalize( DirectX::XMVectorSubtract( targetVect, eyeVect ) );
        dirVect = DirectX::XMVectorScale( dirVect, dist );

        DirectX::XMFLOAT3 dirFloat3;
        DirectX::XMStoreFloat3( &dirFloat3, dirVect );

        translate( dirFloat3 );
    }

    void CameraComponent::moveBackward( float dist )
    {
        moveForward( -dist );
    }

    void CameraComponent::moveLeft( float dist )
    {
        moveRight( -dist );
    }

    void CameraComponent::moveRight( float dist )
    {
        DirectX::XMVECTOR eyeVect    = DirectX::XMLoadFloat3( &m_eye );
        DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3( &m_target );
        DirectX::XMVECTOR dirVect    = DirectX::XMVectorSubtract( targetVect, eyeVect );

        DirectX::XMVECTOR upVect    = DirectX::XMLoadFloat3( &m_camUp );
        DirectX::XMVECTOR rightVect = DirectX::XMVector3Normalize( DirectX::XMVector3Cross( dirVect, upVect ) );
        rightVect = DirectX::XMVectorScale( rightVect, dist );

        DirectX::XMFLOAT3 rightFloat3;
        DirectX::XMStoreFloat3( &rightFloat3, rightVect );

        translate( rightFloat3 );
    }

    void CameraComponent::translate( const DirectX::XMFLOAT3 &deltaPos )
    {

        DirectX::XMFLOAT3 newPos = DirectX::XMFLOAT3( m_eye.x + deltaPos.x,
                                                      m_eye.y + deltaPos.y,
                                                      m_eye.z + deltaPos.z );
        DirectX::XMFLOAT3 newTarget = DirectX::XMFLOAT3( m_target.x + deltaPos.x,
                                                         m_target.y + deltaPos.y,
                                                         m_target.z + deltaPos.z );
        lookAt( newPos, newTarget, m_camUp );
    }

    void CameraComponent::rotateUp( float degrees )
    {
        DirectX::XMVECTOR upVect     = DirectX::XMLoadFloat3( &m_camUp );
        DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3( &m_target );
        DirectX::XMVECTOR eyeVect    = DirectX::XMLoadFloat3( &m_eye );
        DirectX::XMVECTOR dirVect    = DirectX::XMVector3Normalize( DirectX::XMVectorSubtract( targetVect, eyeVect ) );

        DirectX::XMVECTOR rotAxis = DirectX::XMVector3Cross( dirVect, upVect );
        DirectX::XMMATRIX rotMat  = DirectX::XMMatrixRotationAxis( rotAxis, DirectX::XMConvertToRadians( degrees ) );

        DirectX::XMVECTOR newTarget = DirectX::XMVectorAdd( DirectX::XMVector3Transform( dirVect, rotMat ), eyeVect );
        DirectX::XMStoreFloat3( &m_target, newTarget );


        lookAt( m_eye, m_target, m_camUp );
    }

    void CameraComponent::rotateDown( float degrees )
    {
        rotateUp( -degrees );
    }

    void CameraComponent::rotateLeft( float degrees )
    {
        DirectX::XMVECTOR upVect     = DirectX::XMLoadFloat3( &m_camUp );
        DirectX::XMVECTOR targetVect = DirectX::XMLoadFloat3( &m_target );
        DirectX::XMVECTOR eyeVect    = DirectX::XMLoadFloat3( &m_eye );
        DirectX::XMVECTOR dirVect    = DirectX::XMVectorSubtract( targetVect, eyeVect );

        //TODO: fix assertion error when dirVect == (0.0, -1.0, 0.0) and upVect == (0.0, 1.0, 0.0)
        DirectX::XMVECTOR rightVect = DirectX::XMVector3Cross( dirVect, upVect );
        DirectX::XMVECTOR rotAxis   = DirectX::XMVector3Normalize( DirectX::XMVector3Cross( rightVect, dirVect ) );
        DirectX::XMMATRIX rotMat    = DirectX::XMMatrixRotationAxis( rotAxis, DirectX::XMConvertToRadians( degrees ) );

        DirectX::XMVECTOR newTarget = DirectX::XMVectorAdd( DirectX::XMVector3Transform( dirVect, rotMat ), eyeVect );
        DirectX::XMStoreFloat3( &m_target, newTarget );

        lookAt( m_eye, m_target, m_camUp );
    }

    void CameraComponent::rotateRight( float degrees )
    {
        rotateLeft( -degrees );
    }
}
