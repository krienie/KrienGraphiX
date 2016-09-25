
#pragma once

#include <DirectXMath.h>
#include <vector>

namespace kgx
{
	class VertexInputLayout;

	typedef unsigned int GeometryID;

	class Geometry
	{
		public:
			class Iterator
			{
				public:
					Iterator( const Geometry *geometry, int stride, int vertexSize );
					Iterator( const Geometry *geometry, int stride, int vertexSize, const std::vector<unsigned int>::const_iterator &indexIterator );
					//~Iterator();

					bool operator==( const Iterator &rhs );
					bool operator!=( const Iterator &rhs );
					Iterator& operator++();
					Iterator operator++(int);
					Iterator& operator--();
					Iterator operator--(int);

					DirectX::XMFLOAT3 m_vertex1;
					DirectX::XMFLOAT3 m_vertex2;
					DirectX::XMFLOAT3 m_vertex3;

				private:
					void getTriangleFromIterator( std::vector<unsigned int>::const_iterator &indexIterator );

					const Geometry *m_geom;
					int m_typeStride;
					int m_vertexSize;
					std::vector<unsigned int>::const_iterator m_indexIterator;
			};

			Geometry( const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const VertexInputLayout *inputDescriptor );
			//~Geometry();

			Iterator positionCbegin() const;
			Iterator positionCend() const;
			Iterator texCoordCbegin() const;
			Iterator texCoordCend() const;
			Iterator normalsCbegin() const;
			Iterator normalsCend() const;
			Iterator tangentsCbegin() const;
			Iterator tangentsCend() const;

		private:
			int m_posStride;
			int m_texCoordStride;
			int m_normalsStride;
			int m_tangentsStride;
			int m_vertexSize;

			std::vector<float> m_vertices;
			std::vector<unsigned int> m_indices;
	};
}
