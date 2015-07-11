
#pragma once

#include <DirectXMath.h>

namespace kgx
{
	class MovableObject
	{
		public:
			MovableObject();
			virtual ~MovableObject();

			DirectX::XMFLOAT4X4 getModelMatrix() const;

			virtual void translate( float deltaX, float deltaY, float deltaZ );
			virtual void scale( float deltaScale );
			virtual void scale( float deltaX, float deltaY, float deltaZ );
			virtual void setScale( float newScale );
			virtual void setScale( float xNewScale, float yNewScale, float zNewScale );

		private:
			DirectX::XMFLOAT4X4 m_modelMatrix;

			float m_xPos;
			float m_yPos;
			float m_zPos;
			float m_xScale;
			float m_yScale;
			float m_zScale;

	};
}
