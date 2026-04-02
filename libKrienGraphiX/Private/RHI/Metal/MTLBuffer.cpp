
#include "MTLBuffer.h"

#include "MTLCommandQueue.h"
#include "MTLRenderHardwareInterface.h"
#include "Private/Core/RenderThread.h"

namespace kgx::RHI
{
MTLBuffer::MTLBuffer(const RHIBufferDescriptor& descriptor)
	: RHIBuffer(descriptor)
{
	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice()->getNativeDevice();

	//const MTL::ResourceOptions storageMode = descriptor.isDynamic
	//											? MTL::ResourceStorageModeShared
	//											: MTL::ResourceStorageModePrivate;

	//TODO(KL): Add a staging buffer system so non-dynamic buffers can be made private
	constexpr MTL::ResourceOptions storageMode = MTL::ResourceStorageModeShared;
	
	uint32_t totalBufferSize = descriptor.bufferSize;

	if (getDescriptor().isDynamic)
	{
		// Dynamic buffers are buffered for multiple frames to avoid race conditions
		totalBufferSize *= core::RenderThread::maxNumBufferedFrames;
	}

	if (descriptor.initialData)
	{
		const int numBufferedFrames = getDescriptor().isDynamic ? core::RenderThread::maxNumBufferedFrames : 1;
		
		std::vector<uint8_t> localInitialData;
		localInitialData.resize(descriptor.bufferSize * numBufferedFrames);

		for (int i = 0; i < numBufferedFrames; i++)
		{
			const uint8_t offset = i * descriptor.bufferSize;
			uint8_t* bufferData = localInitialData.data() + offset;
			memcpy(bufferData, descriptor.initialData, descriptor.bufferSize);
		}

		mResource = NS::TransferPtr(mtlDevice->newBuffer(localInitialData.data(), totalBufferSize, storageMode));
	}
	else
	{
		mResource = NS::TransferPtr(mtlDevice->newBuffer(totalBufferSize, storageMode));
	}

	//TODO(KL): All buffers are temporarily added to the global commandqueue residency set
	MTLCommandQueue* mtlCommandQueue = rcCast(core::gRenderThread->getCommandQueuePtr());

	mtlCommandQueue->getResidencySet()->addAllocation(mResource.get());
	mtlCommandQueue->getResidencySet()->commit();
}

MTL::GPUAddress MTLBuffer::getGPUAddress() const
{
	size_t frameOffset = 0;

	if (getDescriptor().isDynamic)
	{
		frameOffset = core::gRenderThread->getBufferedFrameIndex() * bufferSize();
	}

	return mResource->gpuAddress() + frameOffset;
}

void* MTLBuffer::mappedDataPtr() const
{
	const size_t offset = core::gRenderThread->getBufferedFrameIndex() * bufferSize();
	return static_cast<uint8_t*>(mResource->contents()) + offset;
}

void* MTLBuffer::mapImpl(MapType type)
{
	assert(getDescriptor().isDynamic);
	return mappedDataPtr();
}

void MTLBuffer::unmapImpl()
{
	//Not needed for Metal
}
}
