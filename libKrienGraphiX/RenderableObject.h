
#pragma once

#include <string>
#include <d3d11.h>

#include "Defines.h"


namespace kgx
{
	class MeshBuffer;

	class RenderableObject
	{
		public:
			RenderableObject( const std::string &name, D3D11_PRIMITIVE_TOPOLOGY topology, MeshBuffer *meshBuffer,
							  UINT indexCount, UINT startIndex, UINT baseVertex );
			~RenderableObject();

			void setPosition( float xPos, float yPos, float zPos );
			void setScale( float xScale, float yScale, float zScale );

			std::string getName() const;
			D3D11_PRIMITIVE_TOPOLOGY getTopology() const;
			const MeshBuffer* getMeshBuffer() const;
			UINT getIndexCount() const;
			UINT getStartIndex() const;
			UINT getBaseVertex() const;
			void setMaterial( const Material &material );
			Material getMaterial() const;

			DirectX::XMFLOAT4X4 getModelMatrix();
			DirectX::XMFLOAT4X4 getNormalMatrix();

		private:
			bool m_isDirty;

			std::string m_name;

			D3D11_PRIMITIVE_TOPOLOGY m_topology;
			MeshBuffer* m_meshBuffer;
		
			UINT m_indexCount;
			UINT m_startIndex;
			UINT m_baseVertex;

			Material m_material;
		
			float m_xPos;
			float m_yPos;
			float m_zPos;
			float m_xScale;
			float m_yScale;
			float m_zScale;

			DirectX::XMFLOAT4X4 m_modelMatrix;
			DirectX::XMFLOAT4X4 m_normalMatrix;
	};
}
