
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
	RenderThread* renderThread = RenderCore::get()->getRenderThreadPtr();
	mCommandList = renderThread->getCommandListPoolPtr()->getResource();
	mCommandAllocator = renderThread->getCommandAllocatorPoolPtr()->getResource();

	mCommandList->reset(mCommandAllocator);
}

ImmediateCommandContext::~ImmediateCommandContext()
{
	mCommandList->close();

	const auto* renderThread = core::RenderCore::get()->getRenderThreadPtr();
	renderThread->getCommandQueuePtr()->executeCommandList(mCommandList);
	renderThread->getCommandQueuePtr()->flushQueue();

	mCommandAllocator->reset();

	mCommandList->release();
	mCommandAllocator->release();
}

FrameCommandContext::FrameCommandContext(uint64_t frameNumber, RHI::RHIFence* frameFence)
	: mFrameNumber(frameNumber), mFrameFence(frameFence)
{
	RenderThread* renderThread = RenderCore::get()->getRenderThreadPtr();
	mCommandList = renderThread->getCommandListPoolPtr()->getResource();
	mCommandAllocator = renderThread->getCommandAllocatorPoolPtr()->getResource();

	mCommandList->reset(mCommandAllocator);
}

FrameCommandContext::~FrameCommandContext()
{
	mCommandAllocator->reset();
	mCommandAllocator->release();
}

void FrameCommandContext::endFrame() const
{
	mCommandList->close();

	const auto* renderThread = RenderCore::get()->getRenderThreadPtr();
	renderThread->getCommandQueuePtr()->executeCommandList(mCommandList);

	mFrameFence->queueSignal(mFrameNumber);

	mCommandList->release();
}

RenderThread::RenderThread()
	: mCommandThread(std::make_unique<CommandThread>(1)),
		mCommandListPool(nullptr),
		mShaderCache(nullptr)
{
#ifdef WIN32
	RHI::PlatformRHI = std::make_unique<RHI::DX12RenderHardwareInterface>();
#elif defined(__APPLE__)
	RHI::PlatformRHI = std::make_unique<RHI::MTLRenderHardwareInterface>();
#else
	static_assert(false, "Unsupported platform");
#endif

	assert(RHI::PlatformRHI != nullptr && "Error creating RHI!");

#ifdef __APPLE__
	//TODO(KL): Temporary to avoid crashes because of unimplemented code
	return;
#endif

	mCommandQueue = RHI::PlatformRHI->createCommandQueue();

	mCommandListPool = std::make_unique<CommandListPool>(5, []()
	{
		return RHI::PlatformRHI->createGraphicsCommandList(nullptr);
	});

	mCommandAllocatorPool = std::make_unique<CommandAllocatorPool>(5, []()
	{
		return RHI::PlatformRHI->createCommandAllocator();
	});

	mShaderCache = std::make_unique<rendering::KGXShaderCache>();
	mFrameFence = RHI::PlatformRHI->createFence();
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
	// Release any frame resources that have already been processed
	while (!mFrameResources.empty() && mFrameFence->getCurrentValue() >= mFrameResources.front()->getFrameNumber())
	{
		mFrameResources.pop();
	}

	++mCurrentFrame;
	mFrameResources.push(std::make_unique<FrameCommandContext>(mCurrentFrame, mFrameFence.get()));

	constexpr int maxFrameResourcesSize = 3;
	if (mFrameResources.size() > maxFrameResourcesSize)
	{
		mFrameFence->waitForValue(mFrameResources.front()->getFrameNumber());
		mFrameResources.pop();
	}
}

void RenderThread::enqueueCommand(RenderCommand cmd) const
{
	mCommandThread->enqueueCommand(std::move(cmd));
}

void RenderThread::flush() const
{
	mCommandThread->flush();
	mCommandQueue->flushQueue();
}

void RenderThread::shutdown()
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
	mCommandThread.reset();
	RHI::PlatformRHI.reset();
}
}
