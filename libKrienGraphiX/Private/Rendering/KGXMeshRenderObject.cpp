
#include "KGXMeshRenderObject.h"

#include "Private/Core/RenderCore.h"
#include "Private/RHI/RenderHardwareInterface.h"

namespace kgx::rendering
{
KGXMeshRenderObject::KGXMeshRenderObject(const RawMeshData& rawMeshData)
	: mTransform(1.0), mRawMeshData(rawMeshData)
{
}

void KGXMeshRenderObject::createRenderResources()
{
	const unsigned int vbByteSize = static_cast<unsigned int>(mRawMeshData.vertices.size()) * sizeof(Vertex);
	const unsigned int ibByteSize = static_cast<unsigned int>(mRawMeshData.indices.size()) * sizeof(std::uint16_t);

	core::ImmediateCommandContext immediateCommandContext;

	const RHI::RHIBufferDescriptor vertexBufferDesc
	{
		.name = "VertexBuffer",
		.bufferSize = vbByteSize,
		.bufferRegister = 0,
		.isBufferAligned = false,
		.isDynamic = false,
		.initialData = mRawMeshData.vertices.data(),
		.flags = RHI::RHIResource::CreationFlags::VertexBuffer
	};

	mVertexBuffer = RHI::PlatformRHI->createBuffer(immediateCommandContext.getCommandList(), vertexBufferDesc);

	const RHI::RHIBufferDescriptor indexBufferDesc
	{
		.name = "IndexBuffer",
		.bufferSize = ibByteSize,
		.bufferRegister = 0,
		.isBufferAligned = false,
		.isDynamic = false,
		
		.initialData = mRawMeshData.indices.data(),
		.flags = RHI::RHIResource::CreationFlags::IndexBuffer
	};

	mIndexBuffer = RHI::PlatformRHI->createBuffer(immediateCommandContext.getCommandList(), indexBufferDesc);
}

void KGXMeshRenderObject::updateTransform(const math::Matrix4X4& newTransform)
{
	mTransform = newTransform;
}
}
