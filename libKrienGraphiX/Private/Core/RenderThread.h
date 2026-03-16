
#pragma once

#include <functional>
#include <memory>
#include <queue>

#include "ResourcePool.h"
#include "Private/Rendering/KGXShaderCache.h"
#include "Private/RHI/RHICommandAllocator.h"
#include "Private/RHI/RHIGraphicsCommandList.h"
#include "Private/RHI/RHIFence.h"

#include "Private/RHI/RHISwapChain.h"

namespace kgx::core
{
class CommandThread;

class ImmediateCommandContext final
{
public:
	ImmediateCommandContext();
	~ImmediateCommandContext();

	RHI::RHIGraphicsCommandList* getCommandList() const { return mCommandList; }

private:
	RHI::RHIGraphicsCommandList* mCommandList;
	RHI::RHICommandAllocator* mCommandAllocator;
};

class FrameCommandContext final
{
public:
	FrameCommandContext(uint64_t frameNumber, RHI::RHIFence* frameFence);
	~FrameCommandContext();

	RHI::RHIGraphicsCommandList* getCommandList() const { return mCommandList; }

	uint64_t getFrameNumber() const { return mFrameNumber; }
	void endFrame() const;

private:
	uint64_t mFrameNumber;
	RHI::RHIFence* mFrameFence;
	RHI::RHIGraphicsCommandList* mCommandList;
	RHI::RHICommandAllocator* mCommandAllocator;
};

class RenderThread final
{
public:
	using RenderCommand = std::function<void()>;
	using CommandListPool = ResourcePool<RHI::RHIGraphicsCommandList>;
	using CommandAllocatorPool = ResourcePool<RHI::RHICommandAllocator>;

	RenderThread();
	~RenderThread() = default;

	RenderThread(const RenderThread&) noexcept            = delete;
	RenderThread(RenderThread&&) noexcept                 = delete;
	RenderThread& operator=(const RenderThread&) noexcept = delete;
	RenderThread& operator=(RenderThread&&) noexcept      = delete;

	[[nodiscard]] RHI::RHICommandQueue* getCommandQueuePtr() const;
	[[nodiscard]] rendering::KGXShaderCache* getShaderCachePtr() const;
	[[nodiscard]] CommandListPool* getCommandListPoolPtr() const;
	[[nodiscard]] CommandAllocatorPool* getCommandAllocatorPoolPtr() const;

	[[nodiscard]] FrameCommandContext* getCurrentFrameContext() const;
	[[nodiscard]] RHI::RHIGraphicsCommandList* getCurrentFrameCommandList() const;

	void nextFrame();
	void enqueueCommand(RenderCommand cmd) const;
	void flush() const;
	void shutdown();

private:
	std::unique_ptr<CommandThread> mCommandThread;

	std::unique_ptr<RHI::RHICommandQueue> mCommandQueue;
	std::unique_ptr<CommandListPool> mCommandListPool;
	std::unique_ptr<ResourcePool<RHI::RHICommandAllocator>> mCommandAllocatorPool;

	std::unique_ptr<rendering::KGXShaderCache> mShaderCache;

	uint64_t mCurrentFrame = -1;
	std::unique_ptr<RHI::RHIFence> mFrameFence;
	std::queue<std::unique_ptr<FrameCommandContext>> mFrameResources;
};
}
