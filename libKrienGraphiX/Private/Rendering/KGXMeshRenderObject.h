
#pragma once
#include <DirectXMath.h>
#include <memory>

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

private:
	RawMeshData mRawMeshData;
	std::unique_ptr<RHI::RHIBuffer> mVertexBuffer;
	std::unique_ptr<RHI::RHIBuffer> mIndexBuffer;
};
}
