
#pragma once
#include <memory>

#include "KrienGraphiX/Math/MathDefines.h"
#include "Private/RHI/RHIBuffer.h"

namespace kgx::rendering
{
struct Vertex
{
	math::Vector3 Pos;
	math::Vector4 Color;
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

	[[nodiscard]] const math::Matrix4X4& getTransform() const { return mTransform; }
	[[nodiscard]] RHI::RHIBuffer* getVertexBuffer() const { return mVertexBuffer.get(); }
	[[nodiscard]] RHI::RHIBuffer* getIndexBuffer() const { return mIndexBuffer.get(); }
	[[nodiscard]] size_t getNumVertices() const { return mRawMeshData.vertices.size(); }
	[[nodiscard]] size_t getNumIndices() const { return mRawMeshData.indices.size(); }

private:
	math::Matrix4X4 mTransform;

	RawMeshData mRawMeshData;
	std::unique_ptr<RHI::RHIBuffer> mVertexBuffer;
	std::unique_ptr<RHI::RHIBuffer> mIndexBuffer;
};
}
