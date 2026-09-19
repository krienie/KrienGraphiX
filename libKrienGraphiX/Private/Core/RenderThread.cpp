
#include "RenderThread.h"

#include <cassert>

#include "CommandThread.h"
#include "RenderCore.h"
#include "KrienGraphiX/Core/Logging.h"
#include "Private/RHI/RHIRenderContext.h"

#ifdef WIN32
#include "Private/RHI/D3D12/DX12RenderHardwareInterface.h"
#elif defined(__APPLE__)
#include "Private/RHI/Metal/MTLRenderHardwareInterface.h"
#endif

namespace kgx::core
{
ImmediateCommandContext::ImmediateCommandContext()
{
	KGXLOG_TRACE("ImmediateCommandContext");

	mRenderContext = gRenderThread->getRHIPlatformPtr()->getRenderContext();
	mRenderContext->reset();
}

ImmediateCommandContext::~ImmediateCommandContext()
{
	mRenderContext->close();

	constexpr bool waitForCompletion = true;
	mRenderContext->execute(waitForCompletion);

	mRenderContext->release();
}

FrameCommandContext::FrameCommandContext(uint64_t frameNumber, RHI::RHIFence* frameFence)
	: mFrameNumber(frameNumber), mFrameFence(frameFence)
{
	KGXLOG_TRACE("FrameCommandContext {}", frameNumber);

	mRenderContext = gRenderThread->getRHIPlatformPtr()->getRenderContext();
	mRenderContext->reset();
}

FrameCommandContext::~FrameCommandContext()
{
	mRenderContext->release();
}

void FrameCommandContext::endFrame() const
{
	mRenderContext->close();

	constexpr bool waitForCompletion = false;
	mRenderContext->execute(waitForCompletion);

	mFrameFence->queueSignal(mFrameNumber);
}

RenderThread::RenderThread()
	: mCommandThread(std::make_unique<CommandThread>(1)),
		mShaderCache(nullptr)
{
#ifdef WIN32
	RHI::gPlatformRHI = std::make_unique<RHI::DX12RenderHardwareInterface>();
#elif defined(__APPLE__)
	RHI::gPlatformRHI = std::make_unique<RHI::MTLRenderHardwareInterface>();
#else
	static_assert(false, "Unsupported platform");
#endif

	KGXLOG_CRITICAL_IF(RHI::gPlatformRHI == nullptr, "Error creating RHI!");
	assert(RHI::gPlatformRHI != nullptr && "Error creating RHI!");

	mShaderCache = std::make_unique<rendering::KGXShaderCache>();
	mRHIPlatform = RHI::gPlatformRHI->createPlatform();
	mRHIPlatform->init();

	mFrameFence = RHI::gPlatformRHI->createFence(*mRHIPlatform);
}

rendering::KGXShaderCache* RenderThread::getShaderCachePtr() const
{
	return mShaderCache.get();
}

RHI::RHIPlatform* RenderThread::getRHIPlatformPtr() const
{
	return mRHIPlatform.get();
}

FrameCommandContext* RenderThread::getCurrentFrameContext() const
{
	return mFrameResources.back().get();
}

void RenderThread::nextFrame()
{
	if (mFrameResources.size() >= maxNumBufferedFrames)
	{
		KGXLOG_TRACE("Waiting for frame fence {}", mFrameResources.front()->getFrameNumber());
		mFrameFence->waitForValue(mFrameResources.front()->getFrameNumber());
		mFrameResources.pop();
	}

	//TODO(KL): See if I can get rid of the make_unique here. Just use raw memory without constantly allocating new every frame.
	++mCurrentFrame;
	mFrameResources.push(std::make_unique<FrameCommandContext>(mCurrentFrame, mFrameFence.get()));
}

void RenderThread::shutdown()
{
	// Add the shutdown code as a render command as some graphics APIs use strictly thread-local resources.
	mCommandThread->enqueueCommandAndStop([this]()
	{
		{
			std::queue<std::unique_ptr<FrameCommandContext>> emptyQueue;
			mFrameResources.swap(emptyQueue);
		}

		for (int i = 0; i < mFrameResources.size(); ++i)
		{
			mFrameResources.pop();
		}

		mRHIPlatform.reset();
		mFrameFence.reset();
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
