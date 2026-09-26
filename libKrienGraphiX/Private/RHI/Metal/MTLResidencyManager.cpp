
#include "MTLResidencyManager.h"

#include "MTLCommandQueue.h"
#include "MTLPlatform.h"
#include "Private/Core/RenderThread.h"

namespace kgx::RHI
{
void MTLResidencyManager::addGlobalResidency(const MTL::Allocation* allocation)
{
	core::gRenderThread->enqueueCommand([allocation]()
	{
		auto* mtlPlatform = static_cast<MTLPlatform*>(core::gRenderThread->getRHIPlatformPtr());
		MTLCommandQueue& mtlCommandQueue = mtlPlatform->getCommandQueue();
		mtlCommandQueue.addGlobalResidency(allocation);
	});
}

void MTLResidencyManager::removeGlobalResidency(const MTL::Allocation* allocation)
{
	core::gRenderThread->enqueueCommand([allocation]()
	{
		auto* mtlPlatform = static_cast<MTLPlatform*>(core::gRenderThread->getRHIPlatformPtr());
		MTLCommandQueue& mtlCommandQueue = mtlPlatform->getCommandQueue();
		mtlCommandQueue.removeGlobalResidency(allocation);
	});
}
}
