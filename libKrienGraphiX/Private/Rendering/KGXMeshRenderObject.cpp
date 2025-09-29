
#include "KGXMeshRenderObject.h"

#include <DirectXMath.h>

#include "Private/Core/RenderCore.h"
#include "Private/RHI/RenderHardwareInterface.h"

namespace kgx::rendering
{
using namespace DirectX;

KGXMeshRenderObject::KGXMeshRenderObject(const RawMeshData& rawMeshData)
	: mTransform(math::IdentityMatrix4X4), mRawMeshData(rawMeshData)
{
}

void KGXMeshRenderObject::createRenderResources()
{
	const unsigned int vbByteSize = static_cast<unsigned int>(mRawMeshData.vertices.size()) * sizeof(Vertex);
	const unsigned int ibByteSize = static_cast<unsigned int>(mRawMeshData.indices.size()) * sizeof(std::uint16_t);

	const core::RenderThread* renderThreadPtr = core::RenderCore::get()->getRenderThreadPtr();

	RHI::RHIGraphicsCommandList* commandList = renderThreadPtr->getGraphicsCommandListPtr();

	const RHI::RHIBufferDescriptor vertexBufferDesc
	{
		.name = "VertexBuffer",
		.bufferSize = vbByteSize,
		.bufferRegister = 0,
		.isBufferAligned = false,
		.isDynamic = false,
		.initialData = mRawMeshData.vertices.data(),
		.flags = RHI::RHIResource::CreationFlags::None
	};

	mVertexBuffer = RHI::PlatformRHI->createBuffer(commandList, vertexBufferDesc);


	const RHI::RHIBufferDescriptor indexBufferDesc
	{
		.name = "IndexBuffer",
		.bufferSize = ibByteSize,
		.bufferRegister = 0,
		.isBufferAligned = false,
		.isDynamic = false,
		
		.initialData = mRawMeshData.indices.data(),
		.flags = RHI::RHIResource::CreationFlags::None
	};

	mIndexBuffer = RHI::PlatformRHI->createBuffer(commandList, indexBufferDesc);
}

void KGXMeshRenderObject::updateTransform(const math::Matrix4X4& newTransform)
{
	mTransform = newTransform;
}
}
