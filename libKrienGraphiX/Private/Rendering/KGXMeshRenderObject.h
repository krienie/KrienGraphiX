
#pragma once
#include <memory>

#include "Private/RHI/RHIBuffer.h"

namespace kgx::rendering
{

class KGXMeshRenderObject
{
public:
	KGXMeshRenderObject();
	~KGXMeshRenderObject() = default;

	void createRenderResources();

private:
	std::unique_ptr<RHI::RHIBuffer> mVertexBuffer;
	std::unique_ptr<RHI::RHIBuffer> mIndexBuffer;
};
}
