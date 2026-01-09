
#pragma once
#include <DirectXMath.h>
#include <memory>

#include "Private/Math/MathDefines.h"
#include "Private/RHI/RHIBuffer.h"

namespace kgx::rendering
{
struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

class KGXMeshRenderObject
{
public:
	struct RawMeshData
	{
		std::vector<Vertex> vertices;
		std::vector<std::uint16_t> indices;
	};

	KGXMeshRenderObject(const RawMeshData& rawMeshData);
	~KGXMeshRenderObject() = default;

	void createRenderResources();
	void updateTransform(const math::Matrix4X4& newTransform);

	const math::Matrix4X4& getTransform() const { return mTransform; }
	RHI::RHIBuffer* getVertexBuffer() const { return mVertexBuffer.get(); }
	RHI::RHIBuffer* getIndexBuffer() const { return mIndexBuffer.get(); }
	size_t getNumVertices() const { return mRawMeshData.vertices.size(); }

private:
	math::Matrix4X4 mTransform;

	RawMeshData mRawMeshData;
	std::unique_ptr<RHI::RHIBuffer> mVertexBuffer;
	std::unique_ptr<RHI::RHIBuffer> mIndexBuffer;
};
}
