
#pragma once

#include <functional>
#include <memory>
#include <queue>

#include "CommandThread.h"
#include "Private/Rendering/KGXShaderCache.h"
#include "Private/RHI/RHIFence.h"
#include "Private/RHI/RHIPlatform.h"

#include "Private/RHI/RHISwapChain.h"

namespace kgx::RHI
{
class RHIRenderContext;
}

namespace kgx::core
{
class CommandThread;

class ImmediateCommandContext final
{
public:
	ImmediateCommandContext();
	~ImmediateCommandContext();

	[[nodiscard]]
	RHI::RHIRenderContext* getRenderContext() const { return mRenderContext; }

private:
	RHI::RHIRenderContext* mRenderContext;
};

class FrameCommandContext final
{
public:
	FrameCommandContext(uint64_t frameNumber, RHI::RHIFence* frameFence);
	~FrameCommandContext();

	[[nodiscard]] RHI::RHIRenderContext* getRenderContext() const { return mRenderContext; }
	[[nodiscard]] uint64_t getFrameNumber() const { return mFrameNumber; }

	void endFrame() const;

private:
	uint64_t mFrameNumber;
	RHI::RHIFence* mFrameFence;
	RHI::RHIRenderContext* mRenderContext;
};

class DeferredRenderCommand
{
public:
	DeferredRenderCommand(unsigned int numFramesToExecute, CommandThread::ThreadCommand&& deferredCommand);

	void operator()();

private:
	unsigned int mNumFramesToExecute;
	CommandThread::ThreadCommand mCommand;
};

class RenderThread final
{
public:
	using RenderCommand = std::function<void()>;

	RenderThread();

	RenderThread(const RenderThread&) noexcept            = delete;
	RenderThread(RenderThread&&) noexcept                 = delete;
	RenderThread& operator=(const RenderThread&) noexcept = delete;
	RenderThread& operator=(RenderThread&&) noexcept      = delete;

	[[nodiscard]] rendering::KGXShaderCache* getShaderCachePtr() const;
	[[nodiscard]] RHI::RHIPlatform* getRHIPlatformPtr() const;

	[[nodiscard]] FrameCommandContext* getCurrentFrameContext() const;

	void enqueueCommand(CommandThread::ThreadCommand&& cmd) const;

	void nextFrame();
	void shutdown();

	[[nodiscard]]
	uint64_t getCurrentFrameNumber() const { return mCurrentFrame; }

	[[nodiscard]]
	int getBufferedFrameIndex() const;

	static constexpr int maxNumBufferedFrames = 3;
	static_assert(maxNumBufferedFrames == 2 || maxNumBufferedFrames == 3);

private:
	std::unique_ptr<CommandThread> mCommandThread;
	std::unique_ptr<RHI::RHIPlatform> mRHIPlatform;
	std::unique_ptr<rendering::KGXShaderCache> mShaderCache;

	uint64_t mCurrentFrame = -1;
	std::unique_ptr<RHI::RHIFence> mFrameFence;
	std::queue<std::unique_ptr<FrameCommandContext>> mFrameResources;
};

inline std::unique_ptr<RenderThread> gRenderThread = nullptr;
}
