
#include "VertexInputLayout.h"
#include "Geometry.h"

namespace kgx
{
	Geometry::Iterator::Iterator( const Geometry *geometry, int stride, int vertexSize )
		: Iterator(geometry, stride, vertexSize, geometry->m_indices.cbegin())
	{
	}
	Geometry::Iterator::Iterator( const Geometry *geometry, int stride, int vertexSize, const std::vector<unsigned int>::const_iterator &indexIterator )
		: m_vertex1(0.0f, 0.0f, 0.0f), m_vertex2(0.0f, 0.0f, 0.0f), m_vertex3(0.0f, 0.0f, 0.0f),
			m_geom(geometry), m_typeStride(stride), m_vertexSize(vertexSize), m_indexIterator(indexIterator)
	{
		// initializing front position
		getTriangleFromIterator( m_indexIterator );
	}


	/*Geometry::Iterator::~Iterator()
	{
	}*/

	bool Geometry::Iterator::operator==( const Geometry::Iterator &rhs )
	{
		return m_indexIterator == rhs.m_indexIterator;
	}
	bool Geometry::Iterator::operator!=( const Geometry::Iterator &rhs )
	{
		return m_indexIterator != rhs.m_indexIterator;
	}

	Geometry::Iterator& Geometry::Iterator::operator++()
	{
		m_indexIterator += 3;
		if ( m_indexIterator != m_geom->m_indices.cend() )
			getTriangleFromIterator( m_indexIterator );

        return *this;
	}
	Geometry::Iterator Geometry::Iterator::operator++(int)
	{
		Geometry::Iterator result(*this);
		++(*this);
        return result;
	}

	Geometry::Iterator& Geometry::Iterator::operator--()
	{
		m_indexIterator -= 3;
		if ( m_indexIterator != m_geom->m_indices.cbegin() )
			getTriangleFromIterator( m_indexIterator );

        return *this;
	}
	Geometry::Iterator Geometry::Iterator::operator--(int)
	{
		Geometry::Iterator result(*this);
		--(*this);
        return result;
	}

	void Geometry::Iterator::getTriangleFromIterator( std::vector<unsigned int>::const_iterator &indexIterator )
	{
		if ( indexIterator == m_geom->m_indices.cend() )
			return;

		int vertexPos = *indexIterator + m_typeStride;
		m_vertex1 = DirectX::XMFLOAT3( m_geom->m_vertices[vertexPos],
									   m_geom->m_vertices[vertexPos + 1],
									   m_geom->m_vertices[vertexPos + 2] );

		vertexPos = *(indexIterator + 1) * m_vertexSize + m_typeStride;
		m_vertex2 = DirectX::XMFLOAT3( m_geom->m_vertices[vertexPos],
									   m_geom->m_vertices[vertexPos + 1],
									   m_geom->m_vertices[vertexPos + 2] );

		vertexPos = *(indexIterator + 2) * m_vertexSize + m_typeStride;
		m_vertex3 = DirectX::XMFLOAT3( m_geom->m_vertices[vertexPos],
									   m_geom->m_vertices[vertexPos + 1],
									   m_geom->m_vertices[vertexPos + 2] );
	}


	Geometry::Geometry( const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const VertexInputLayout *inputDescriptor )
		: m_posStride(-1), m_texCoordStride(-1), m_normalsStride(-1), m_tangentsStride(-1), m_vertexSize(0),
			m_vertices(vertices), m_indices(indices)
	{
		std::vector<VertexInputLayout::Type> viTypes = inputDescriptor->getInputTypes();
		std::vector<VertexInputLayout::Type>::const_iterator viIt;
		for ( viIt = viTypes.cbegin(); viIt != viTypes.cend(); ++viIt )
		{
			switch ( *viIt )
			{
				case VertexInputLayout::Position:
					m_posStride = m_vertexSize;
					break;
				case VertexInputLayout::TextureCoordinate:
					m_texCoordStride = m_vertexSize;
					break;
				case VertexInputLayout::Normal:
					m_normalsStride = m_vertexSize;
					break;
				case VertexInputLayout::Tangent:
					m_tangentsStride = m_vertexSize;
					break;
			}

			++m_vertexSize;
		}

		m_vertexSize *= 3;
	}

	/*Geometry::~Geometry()
	{
	}*/

	Geometry::Iterator Geometry::positionCbegin() const
	{
		return Iterator( this, m_posStride, m_vertexSize );
	}
	Geometry::Iterator Geometry::positionCend() const
	{
		return Iterator( this, m_posStride, m_vertexSize, m_indices.cend() );
	}

	Geometry::Iterator Geometry::texCoordCbegin() const
	{
		return Iterator( this, m_texCoordStride, m_vertexSize );
	}
	Geometry::Iterator Geometry::texCoordCend() const
	{
		return Iterator( this, m_texCoordStride, m_vertexSize, m_indices.cend() );
	}

	Geometry::Iterator Geometry::normalsCbegin() const
	{
		return Iterator( this, m_normalsStride, m_vertexSize );
	}
	Geometry::Iterator Geometry::normalsCend() const
	{
		return Iterator( this, m_normalsStride, m_vertexSize, m_indices.cend() );
	}

	Geometry::Iterator Geometry::tangentsCbegin() const
	{
		return Iterator( this, m_tangentsStride, m_vertexSize );
	}
	Geometry::Iterator Geometry::tangentsCend() const
	{
		return Iterator( this, m_tangentsStride, m_vertexSize, m_indices.cend() );
	}
}
