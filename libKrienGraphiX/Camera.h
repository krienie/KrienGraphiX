
#pragma once

#include <DirectXMath.h>

namespace kgx
{
	class Scene;

	class Camera
	{
		public:
			Camera( const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up );
			Camera( float fovY, float aspect, float nearZ, float farZ,
					const DirectX::XMFLOAT3 &eye = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
					const DirectX::XMFLOAT3 &target = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),
					const DirectX::XMFLOAT3 &up = DirectX::XMFLOAT3(0.0f, 1.0f,  0.0f) );
			explicit Camera( const Camera &other );
			//~Camera();

			Camera& operator=( const Camera &rhs );

			const DirectX::XMFLOAT4X4& getProjMatrix() const;
			const DirectX::XMFLOAT4X4& getViewMatrix() const;
			const DirectX::XMFLOAT3& getPosition() const;
			const DirectX::XMFLOAT3& getTarget() const;
			float getFOV() const;
			float getAspectRatio() const;
			float getNearZ() const;
			float getFarZ() const;

			void setParentScene( _In_ Scene *scene );

			/** Renders current camera view. Assumes a viewport has already been assigned to the DirectX pipeline */
			void renderCurrentView();

			void lookAt( const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up );
			//TODO: reactivate these functions
			/*void moveForward( float dist );
			void moveBackward( float dist );
			void moveLeft( float dist );
			void moveRight( float dist );
			void translate( const DirectX::XMFLOAT3 &deltaPos );
			//TODO: add support for radians and quaternions
			void rotateUp( float degrees );
			void rotateDown( float degrees );
			void rotateLeft( float degrees );
			void rotateRight( float degrees );*/

		private:
			//TODO: add something that can manipulate the rasterizer

			Scene *m_parentScene;

			DirectX::XMFLOAT4X4 m_projMatrix;
			DirectX::XMFLOAT4X4 m_viewMatrix;

			DirectX::XMFLOAT3 m_position;
			DirectX::XMFLOAT3 m_target;
			DirectX::XMFLOAT3 m_camUp;

			float m_fov;
			float m_aspectRatio;
			float m_nearZ;
			float m_farZ;
	};
}
