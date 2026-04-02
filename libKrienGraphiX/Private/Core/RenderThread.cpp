
#include "RenderThread.h"

#include "RenderCore.h"

#ifdef WIN32
#include "Private/RHI/D3D12/DX12RenderHardwareInterface.h"
#elif defined(__APPLE__)
#include "Private/RHI/Metal/MTLRenderHardwareInterface.h"
#endif

#include "CommandThread.h"

#include <cassert>
#include <utility>

namespace kgx::core
{
ImmediateCommandContext::ImmediateCommandContext()
{
	mCommandList = gRenderThread->getCommandListPoolPtr()->getResource();
	mCommandAllocator = gRenderThread->getCommandAllocatorPoolPtr()->getResource();

	mCommandList->reset(mCommandAllocator, nullptr);
}

ImmediateCommandContext::~ImmediateCommandContext()
{
	mCommandList->close();

	gRenderThread->getCommandQueuePtr()->executeCommandList(mCommandList);
	gRenderThread->getCommandQueuePtr()->flushQueue();

	mCommandAllocator->reset();

	mCommandList->release();
	mCommandAllocator->release();
}

FrameCommandContext::FrameCommandContext(uint64_t frameNumber, RHI::RHIFence* frameFence)
	: mFrameNumber(frameNumber), mFrameFence(frameFence)
{
	mCommandList = gRenderThread->getCommandListPoolPtr()->getResource();
	mCommandAllocator = gRenderThread->getCommandAllocatorPoolPtr()->getResource();

	mCommandList->reset(mCommandAllocator, nullptr);
}

FrameCommandContext::~FrameCommandContext()
{
	mCommandList->release();
	mCommandAllocator->reset();
	mCommandAllocator->release();
}

void FrameCommandContext::endFrame() const
{
	mCommandList->close();

	gRenderThread->getCommandQueuePtr()->executeCommandList(mCommandList);

	mFrameFence->queueSignal(mFrameNumber);
}

RenderThread::RenderThread()
	: mCommandThread(std::make_unique<CommandThread>(1)),
		mCommandListPool(nullptr),
		mShaderCache(nullptr)
{
#ifdef WIN32
	RHI::gPlatformRHI = std::make_unique<RHI::DX12RenderHardwareInterface>();
#elif defined(__APPLE__)
	RHI::gPlatformRHI = std::make_unique<RHI::MTLRenderHardwareInterface>();
#else
	static_assert(false, "Unsupported platform");
#endif

	assert(RHI::gPlatformRHI != nullptr && "Error creating RHI!");

	mCommandQueue = RHI::gPlatformRHI->createCommandQueue();

	mCommandListPool = std::make_unique<CommandListPool>(5, []()
	{
		return RHI::gPlatformRHI->createGraphicsCommandList(nullptr);
	});

	mCommandAllocatorPool = std::make_unique<CommandAllocatorPool>(5, []()
	{
		return RHI::gPlatformRHI->createCommandAllocator();
	});

	mShaderCache = std::make_unique<rendering::KGXShaderCache>();
	mFrameFence = RHI::gPlatformRHI->createFence();
}

RHI::RHICommandQueue* RenderThread::getCommandQueuePtr() const
{
	return mCommandQueue.get();
}

rendering::KGXShaderCache* RenderThread::getShaderCachePtr() const
{
	return mShaderCache.get();
}

RenderThread::CommandListPool* RenderThread::getCommandListPoolPtr() const
{
	return mCommandListPool.get();
}

RenderThread::CommandAllocatorPool* RenderThread::getCommandAllocatorPoolPtr() const
{
	return mCommandAllocatorPool.get();
}

FrameCommandContext* RenderThread::getCurrentFrameContext() const
{
	return mFrameResources.back().get();
}

RHI::RHIGraphicsCommandList* RenderThread::getCurrentFrameCommandList() const
{
	return getCurrentFrameContext()->getCommandList();
}

void RenderThread::nextFrame()
{
	if (mFrameResources.size() >= maxNumBufferedFrames)
	{
		mFrameFence->waitForValue(mFrameResources.front()->getFrameNumber());
		mFrameResources.pop();
	}

	++mCurrentFrame;
	mFrameResources.push(std::make_unique<FrameCommandContext>(mCurrentFrame, mFrameFence.get()));
}

void RenderThread::flush() const
{
	mCommandThread->flush();
	mCommandQueue->flushQueue();
}

void RenderThread::shutdown()
{
	// Add the shutdown code as a render command as some graphics APIs use strictly thread-local resources.
	enqueueCommand([this]()
	{
		{
			std::queue<std::unique_ptr<FrameCommandContext>> emptyQueue;
			mFrameResources.swap(emptyQueue);
		}

		for (int i = 0; i < mFrameResources.size(); ++i)
		{
			mFrameResources.pop();
		}

		mCommandQueue.reset();
		mFrameFence.reset();
		mCommandListPool.reset();
		mCommandAllocatorPool.reset();
		RHI::gPlatformRHI.reset();
	});

	// Shutdown the command thread last
	mCommandThread->flush();
	mCommandThread.reset();
}

int RenderThread::getBufferedFrameIndex() const
{
	return static_cast<int>(getCurrentFrameNumber() % maxNumBufferedFrames);
}
}
