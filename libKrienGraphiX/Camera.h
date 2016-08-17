
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

namespace kgx
{
	class Scene;
	class ShaderProgram;

	class Camera
	{
		public:
			Camera( Scene *parentScene, const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up );
			Camera( Scene *parentScene, float fovY, float aspect, float nearZ, float farZ,
					const DirectX::XMFLOAT3 &eye    = DirectX::XMFLOAT3(0.0f, 0.0f,  0.0f),
					const DirectX::XMFLOAT3 &target = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),
					const DirectX::XMFLOAT3 &up     = DirectX::XMFLOAT3(0.0f, 1.0f,  0.0f) );
			explicit Camera( const Camera &other );
			//~Camera();

			Camera& operator=( const Camera &rhs );

			const DirectX::XMFLOAT4X4& getProjMatrix() const;
			const DirectX::XMFLOAT4X4& getViewMatrix() const;
			const DirectX::XMFLOAT3& getPosition() const;		//TODO: rename to eye
			const DirectX::XMFLOAT3& getTarget() const;
			const DirectX::XMFLOAT3& getUp() const;
			float getFOV() const;
			float getAspectRatio() const;
			float getNearZ() const;
			float getFarZ() const;

			/** Renders current camera view. Assumes a viewport has already been assigned to the DirectX pipeline */
			void renderCurrentView( ID3D11RenderTargetView *rtv, ID3D11DepthStencilView *dsv );

			void lookAt( const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up );
			void moveForward( float dist );
			void moveBackward( float dist );
			void moveLeft( float dist );
			void moveRight( float dist );
			void translate( const DirectX::XMFLOAT3 &deltaPos );
			//TODO: add support for radians and quaternions
			//void rotate( float degreesX, float degreesY, float degreesZ );
			void rotateUp( float degrees );
			void rotateDown( float degrees );
			void rotateLeft( float degrees );
			void rotateRight( float degrees );

		private:
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
