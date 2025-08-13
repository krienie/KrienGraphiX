
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

	mCommandQueue   = RHI::PlatformRHI->createCommandQueue();
	mCommandListAllocator = std::make_unique<CommandListAllocator>(mCommandQueue.get());
	mCommandList    = mCommandListAllocator->createGraphicsCommandList(nullptr);

	mShaderCache = std::make_unique<rendering::KGXShaderCache>(mCommandList);
}

RenderThread::~RenderThread()
{
	if (mCommandList)
	{
		mCommandListAllocator->releaseGraphicsCommandList(mCommandList);
	}

	mCommandThread.reset();
	RHI::PlatformRHI.reset();
}


RHI::RHICommandQueue* RenderThread::getCommandQueuePtr() const
{
	return mCommandQueue.get();
}

RHI::RHIGraphicsCommandList* RenderThread::getGraphicsCommandListPtr() const
{
	return mCommandList;
}

void RenderThread::enqueueCommand(RenderCommand cmd) const
{
	//TODO(KL): encapsulate a lambda here to be able to include things like the commandlist
	
	mCommandThread->enqueueCommand(std::move(cmd));
}

void RenderThread::flush() const
{
	mCommandThread->flush();
}
}
