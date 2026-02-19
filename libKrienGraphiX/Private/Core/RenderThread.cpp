
#include "RenderThread.h"

#ifdef WIN32
#include "Private/RHI/D3D12/DX12RenderHardwareInterface.h"
#endif

#include <cassert>

#include "CommandThread.h"

#include <utility>

namespace kgx::core
{
RenderThread::RenderThread()
	: mCommandThread(std::make_unique<CommandThread>(1)),
		mCommandListAllocator(nullptr),
		mShaderCache(nullptr)
{
#ifdef WIN32
	RHI::PlatformRHI = std::make_unique<RHI::DX12RenderHardwareInterface>();
#else
	static_assert(false, "Only DirectX 12 is currently supported");
#endif

	assert(RHI::PlatformRHI != nullptr && "Error creating RHI!");

	mCommandQueue = RHI::PlatformRHI->createCommandQueue();
	mCommandListAllocator = std::make_unique<CommandListAllocator>();

	mShaderCache = std::make_unique<rendering::KGXShaderCache>();
}

RenderThread::~RenderThread()
{
	mCommandThread.reset();
	RHI::PlatformRHI.reset();
}


RHI::RHICommandQueue* RenderThread::getCommandQueuePtr() const
{
	return mCommandQueue.get();
}

rendering::KGXShaderCache* RenderThread::getShaderCache() const
{
	return mShaderCache.get();
}

RHI::RHIGraphicsCommandListHandle RenderThread::getCommandList() const
{
	return mCommandListAllocator->createGraphicsCommandList(nullptr);
}

void RenderThread::enqueueCommand(RenderCommand cmd) const
{
	mCommandThread->enqueueCommand(std::move(cmd));
}

void RenderThread::flush() const
{
	mCommandThread->flush();
}
}
