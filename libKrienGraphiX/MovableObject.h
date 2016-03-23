
#pragma once

#include <DirectXMath.h>

#include "Object.h"

namespace kgx
{
	//TODO: merge with RenderableObject
	class MovableObject : public Object
	{
		public:
			MovableObject();
			explicit MovableObject( const std::string &name );
			virtual ~MovableObject();

			DirectX::XMFLOAT4X4 getModelMatrix() const;
			DirectX::XMFLOAT4X4 getNormalMatrix() const;

			DirectX::XMFLOAT3 getPosition() const;
			DirectX::XMFLOAT3 getScale() const;

			void translate( float deltaX, float deltaY, float deltaZ );
			void setPosition( float x, float y, float z );
			void scale( float deltaScale );
			void scale( float deltaX, float deltaY, float deltaZ );
			void setScale( float newScale );
			void setScale( float xNewScale, float yNewScale, float zNewScale );

		protected:
			DirectX::XMFLOAT4X4 m_modelMatrix;

		private:
			float m_xPos;
			float m_yPos;
			float m_zPos;
			float m_xScale;
			float m_yScale;
			float m_zScale;

	};
}
