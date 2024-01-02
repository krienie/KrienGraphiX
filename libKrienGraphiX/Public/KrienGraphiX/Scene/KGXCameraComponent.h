
#pragma once

#include <DirectXMath.h>

#include "KGXSceneObjectComponent.h"

namespace kgx
{
    class CameraComponent : public KGXSceneObjectComponent
    {
        public:
            CameraComponent(KGXSceneObject *owner, const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up);
            CameraComponent(KGXSceneObject *owner, float fovY, float aspect, float nearZ, float farZ,
                    const DirectX::XMFLOAT3 &eye = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f ),
                    const DirectX::XMFLOAT3 &target = DirectX::XMFLOAT3( 0.0f, 0.0f, -1.0f ),
                    const DirectX::XMFLOAT3 &up = DirectX::XMFLOAT3( 0.0f, 1.0f, 0.0f ));
            CameraComponent(const CameraComponent &other) noexcept = delete;
            CameraComponent& operator=(const CameraComponent &rhs) = delete;
            CameraComponent(CameraComponent &&other) noexcept = delete;
            CameraComponent&& operator=(CameraComponent &&rhs) = delete;

            const DirectX::XMFLOAT4X4& getProjMatrix() const;
            const DirectX::XMFLOAT4X4& getViewMatrix() const;
            const DirectX::XMFLOAT3& getEye() const;
            const DirectX::XMFLOAT3& getTarget() const;
            const DirectX::XMFLOAT3& getUp() const;
            float getFOV() const;
            float getAspectRatio() const;
            float getNearZ() const;
            float getFarZ() const;

            //TODO(KL): Al deze bewegings dingen kunnen in een MovementComponent worden gezet.
            void lookAt( const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up );
            void moveForward( float dist );
            void moveBackward( float dist );
            void moveLeft( float dist );
            void moveRight( float dist );
            void translate( const DirectX::XMFLOAT3 &deltaPos );
            //TODO: add support for radians and quaternions
            void rotateUp( float degrees );
            void rotateDown( float degrees );
            void rotateLeft( float degrees );
            void rotateRight( float degrees );

        private:
            DirectX::XMFLOAT4X4 m_projMatrix;
            DirectX::XMFLOAT4X4 m_viewMatrix;

            DirectX::XMFLOAT3 m_eye;
            DirectX::XMFLOAT3 m_target;
            DirectX::XMFLOAT3 m_camUp;

            float m_fov;
            float m_aspectRatio;
            float m_nearZ;
            float m_farZ;
    };
}
