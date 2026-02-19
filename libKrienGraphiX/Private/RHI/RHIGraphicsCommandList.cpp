
#include "RHIGraphicsCommandList.h"

#include "Private/Core/CommandListAllocator.h"
#include "Private/Core/RenderCore.h"

namespace kgx::RHI
{
RHIGraphicsCommandList::RHIGraphicsCommandList(core::CommandListAllocator& allocator)
	: mAllocator(&allocator)
{}

void RHIGraphicsCommandList::release()
{
	mAllocator->releaseGraphicsCommandList(this);
}

RHIGraphicsCommandListHandle::RHIGraphicsCommandListHandle(RHIGraphicsCommandList& commandList)
	: mCommandList(&commandList)
{
	mCommandList->reset();
}

RHIGraphicsCommandListHandle::~RHIGraphicsCommandListHandle()
{
	mCommandList->close();

	const auto* renderThread = core::RenderCore::get()->getRenderThreadPtr();
	renderThread->getCommandQueuePtr()->executeCommandList(mCommandList);

	//TODO(KL): Misschien niet nodig?
	renderThread->getCommandQueuePtr()->flushQueue();

	mCommandList->release();
}
}
