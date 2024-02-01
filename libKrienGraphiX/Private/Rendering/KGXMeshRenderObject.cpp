
#include "KGXMeshRenderObject.h"

#include <array>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include "Private/Core/RenderCore.h"
#include "Private/RHI/RenderHardwareInterface.h"

namespace kgx::rendering
{

struct Vertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT4 Color;
};

using namespace DirectX;

KGXMeshRenderObject::KGXMeshRenderObject()
{
    
}

void KGXMeshRenderObject::createRenderResources()
{
    std::array<Vertex, 8> vertices =
    {
        Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
		Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
    };

	std::array<std::uint16_t, 36> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

    const unsigned int vbByteSize = static_cast<unsigned int>(vertices.size()) * sizeof(Vertex);
	const unsigned int ibByteSize = static_cast<unsigned int>(indices.size()) * sizeof(std::uint16_t);

	const core::RenderThread* renderThreadPtr = core::RenderCore::get()->getRenderThreadPtr();

	//TODO(KL): Remove the need to get the graphics device ptr
    RHI::RHIGraphicsDevice* graphicsDevice = renderThreadPtr->getGraphicsDevicePtr();
	RHI::RHIGraphicsCommandList* commandList = renderThreadPtr->getGraphicsCommandListPtr();

	const RHI::RHIBufferDescriptor vertexBufferDesc
	{
		.name = "VertexBuffer",
		.bufferSize = vbByteSize,
		.bufferRegister = 0,
		.isBufferAligned = false,
		.isDynamic = false,
		.initialData = vertices.data(),
		.flags = RHI::RHIResource::CreationFlags::None
	};

	mVertexBuffer = RHI::PlatformRHI->createBuffer(graphicsDevice, commandList, vertexBufferDesc);


	const RHI::RHIBufferDescriptor indexBufferDesc
	{
		.name = "IndexBuffer",
		.bufferSize = ibByteSize,
		.bufferRegister = 0,
		.isBufferAligned = false,
		.isDynamic = false,
		
		.initialData = indices.data(),
		.flags = RHI::RHIResource::CreationFlags::None
	};

	mIndexBuffer = RHI::PlatformRHI->createBuffer(graphicsDevice, commandList, indexBufferDesc);
}
}
