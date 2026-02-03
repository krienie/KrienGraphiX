
#include "DX12Buffer.h"

#include <cassert>

#include "DX12Descriptors.h"
#include "DX12GraphicsCommandList.h"
#include "DX12GraphicsDevice.h"
#include "DX12MemoryUtils.h"
#include "DX12RenderHardwareInterface.h"
#include "Private/Rendering/KGXMeshRenderObject.h"

namespace
{
bool hasFlag(kgx::RHI::RHIResource::CreationFlags flags, kgx::RHI::RHIResource::CreationFlags flagToTest)
{
	return (flags & flagToTest) == flagToTest;
}

kgx::RHI::RHIBufferDescriptor getAlignedBufferDescriptor(const kgx::RHI::RHIBufferDescriptor& descriptor)
{
	kgx::RHI::RHIBufferDescriptor alignedDesc = descriptor;
	if (descriptor.isBufferAligned)
	{
		alignedDesc.bufferSize = static_cast<unsigned>(kgx::RHI::DX12MemoryUtils::alignTo256Bytes(alignedDesc.bufferSize));
	}

	return alignedDesc;
}

D3D12_RESOURCE_STATES getDXResourceStateFromCreationFlag(kgx::RHI::RHIResource::CreationFlags flags)
{
	using namespace kgx::RHI;

	if (hasFlag(flags, RHIResource::IndexBuffer))
	{
		return D3D12_RESOURCE_STATE_INDEX_BUFFER;
	}

	if (hasFlag(flags, RHIResource::VertexBuffer) ||
		hasFlag(flags, RHIResource::ConstantBuffer))
	{
		return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	}

	return D3D12_RESOURCE_STATE_GENERIC_READ;
}
}

namespace kgx::RHI
{
DX12Buffer::DX12Buffer(DX12GraphicsCommandList* commandList, const RHIBufferDescriptor& descriptor)
	: RHIBuffer(getAlignedBufferDescriptor(descriptor)),
		mDescriptor(descriptor)
{
	//TODO(KL): Add descriptor sanitation check

	ID3D12Device* nativeDevice = getDX12RHI()->getDX12Device()->getNativeDevice();

	D3D12_HEAP_TYPE heapType = mDescriptor.isDynamic ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;
	D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_GENERIC_READ;

	//TODO(KL): refactor... this is a bit messy

	//D3D12_RESOURCE_STATE_COMMON
	if (mDescriptor.initialData)
	{
		initialResourceState = D3D12_RESOURCE_STATE_COPY_DEST;
	}

	size_t calculatedBufferSize = bufferSize();

	if (!mDescriptor.isDynamic)
	{
		// Create upload buffer to help with the buffer update
		const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
		const D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(calculatedBufferSize);
		HRESULT res = nativeDevice->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&mBufferUploader));

		if (FAILED(res))
		{
			return;
		}
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> defaultBufferResource;

	// Create the default buffer resource
	const CD3DX12_HEAP_PROPERTIES heapProperties(heapType);
	const D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(calculatedBufferSize);
	HRESULT res = nativeDevice->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&defaultBufferResource));
	
	if (FAILED(res))
	{
		return;
	}

	// Create managed DirectX 12 resource
	mBufferDXResource = std::make_unique<DX12Resource>(defaultBufferResource, nullptr, D3D12_RESOURCE_STATE_COMMON);
	mBufferDXResource->transitionToState(commandList, initialResourceState);

	if (mDescriptor.isDynamic)
	{
		map(MapType::WRITE);
	}

	if (mDescriptor.initialData)
	{
		if (mDescriptor.isDynamic)
		{
			auto mappedData = mappedDataPtr();
			std::memcpy(mappedData, mDescriptor.initialData, calculatedBufferSize);
		}
		else
		{
			D3D12_SUBRESOURCE_DATA subResourceData = {};
			subResourceData.pData = mDescriptor.initialData;
			subResourceData.RowPitch = static_cast<LONG_PTR>(calculatedBufferSize);
			subResourceData.SlicePitch = subResourceData.RowPitch;


			UpdateSubresources<1>(commandList->getCommandList(), mBufferDXResource->getResource().Get(), mBufferUploader.Get(), 0, 0, 1, &subResourceData);
			mBufferDXResource->transitionToState(commandList, getDXResourceStateFromCreationFlag(mDescriptor.flags));
		}
	}

	// Init buffer views if needed
	if (hasFlag(mDescriptor.flags, RHIResource::IndexBuffer))
	{
		mIndexBufferView.BufferLocation = mBufferDXResource->getResource()->GetGPUVirtualAddress();
		mIndexBufferView.SizeInBytes = static_cast<UINT>(bufferSize());
		mIndexBufferView.Format = DXGI_FORMAT_R16_UINT;
	}
	else if (hasFlag(mDescriptor.flags, RHIResource::VertexBuffer))
	{
		mVertexBufferView.BufferLocation = mBufferDXResource->getResource()->GetGPUVirtualAddress();
		mVertexBufferView.SizeInBytes = static_cast<UINT>(bufferSize());
		//TODO(KL): Perhaps move Vertex to a different location or get the vertex stride in a different way somehow
		mVertexBufferView.StrideInBytes = sizeof(rendering::Vertex);
	}
	else if (hasFlag(mDescriptor.flags, RHIResource::ConstantBuffer))
	{
		mConstantBufferView.BufferLocation = mBufferDXResource->getResource()->GetGPUVirtualAddress();
		mConstantBufferView.SizeInBytes = static_cast<UINT>(bufferSize());
	}
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12Buffer::getResource() const
{
	return mBufferDXResource->getResource();
}

const D3D12_INDEX_BUFFER_VIEW* DX12Buffer::getIndexBufferView() const
{
	assert(hasFlag(mDescriptor.flags, RHIResource::IndexBuffer));
	return &mIndexBufferView;
}

const D3D12_VERTEX_BUFFER_VIEW* DX12Buffer::getVertexBufferView() const
{
	assert(hasFlag(mDescriptor.flags, RHIResource::VertexBuffer));
	return &mVertexBufferView;
}

void* DX12Buffer::mapImpl(MapType type)
{
	void* dataPtr;

	// Write only
	if ((type | MapType::WRITE) == MapType::WRITE)
	{
		const CD3DX12_RANGE readRange(0, 0);
		mBufferDXResource->getResource()->Map(0, &readRange, &dataPtr);
	} else
	{
		// TODO(KL): Maybe allow a subset of the buffer to be set for reading. Like, mapSubResource or something
		mBufferDXResource->getResource()->Map(0, nullptr, &dataPtr);
	}

	return dataPtr;
}

void DX12Buffer::unmapImpl()
{
	if ((currentMappedType() | MapType::READ) == MapType::READ)
	{
		const CD3DX12_RANGE writeRange(0, 0);
		mBufferDXResource->getResource()->Unmap(0, &writeRange);
	} else
	{
		mBufferDXResource->getResource()->Unmap(0, nullptr);
	}
}
}