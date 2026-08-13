
#include "KGXMeshRenderObject.h"

#include "KrienGraphiX/Core/Logging.h"
#include "Private/Core/RenderCore.h"
#include "Private/RHI/RenderHardwareInterface.h"

namespace
{
__declspec(align(256u)) struct ConstantBufferData
{
	kgx::math::Matrix4X4 modelMatrix;
};
}

namespace kgx::rendering
{
KGXMeshRenderObject::KGXMeshRenderObject(const RawMeshData& rawMeshData)
	: mTransform(1.0), mRawMeshData(rawMeshData)
{
}

void KGXMeshRenderObject::createRenderResources()
{
	//TODO(KL): Allow to give names to objects so we can log it here
	KGXLOG_INFO("Creating KGXMeshRenderObject");

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

	mVertexBuffer = RHI::gPlatformRHI->createBuffer(immediateCommandContext.getCommandList(), vertexBufferDesc);

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

	mIndexBuffer = RHI::gPlatformRHI->createBuffer(immediateCommandContext.getCommandList(), indexBufferDesc);

	constexpr auto flags = static_cast<RHI::RHIResource::CreationFlags>(
	RHI::RHIResource::ShaderResource | RHI::RHIResource::ConstantBuffer);

	RHI::RHIBufferDescriptor cbDesc
	{
		.name = "MeshRenderObjectConstantBuffer",
		.bufferSize = sizeof(ConstantBufferData),
		.bufferRegister = 0,
		.isBufferAligned = true,
		.isDynamic = true,
		.initialData = nullptr,
		.flags = flags
	};

	mConstantBuffer = RHI::gPlatformRHI->createBuffer(immediateCommandContext.getCommandList(), cbDesc);
}

void KGXMeshRenderObject::updateTransform(const math::Matrix4X4& newTransform)
{
	mTransform = newTransform;
}

void KGXMeshRenderObject::updateConstantBufferData() const
{
	ConstantBufferData cbData { getTransform() };
	memcpy(mConstantBuffer->mappedDataPtr(), &cbData, sizeof(ConstantBufferData));
}
}
