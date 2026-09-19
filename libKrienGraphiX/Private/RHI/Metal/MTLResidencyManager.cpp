
#include "MTLResidencyManager.h"

#include "MTLCommandQueue.h"
#include "MTLPlatform.h"
#include "Private/Core/RenderThread.h"

namespace kgx::RHI
{
void MTLResidencyManager::addGlobalResidency(const RHIResource& resource)
{
	const auto mtlAllocation = static_cast<MTL::Allocation*>(resource.getNativeResource());

	core::gRenderThread->enqueueCommand([mtlAllocation]()
	{
		auto* mtlPlatform = static_cast<MTLPlatform*>(core::gRenderThread->getRHIPlatformPtr());
		MTLCommandQueue& mtlCommandQueue = mtlPlatform->getCommandQueue();
		mtlCommandQueue.addGlobalResidency(mtlAllocation);
	});
}

void MTLResidencyManager::removeGlobalResidency(const RHIResource& resource)
{
	const auto mtlAllocation = static_cast<MTL::Allocation*>(resource.getNativeResource());

	core::gRenderThread->enqueueCommand([mtlAllocation]()
	{
		auto* mtlPlatform = static_cast<MTLPlatform*>(core::gRenderThread->getRHIPlatformPtr());
		MTLCommandQueue& mtlCommandQueue = mtlPlatform->getCommandQueue();
		mtlCommandQueue.removeGlobalResidency(mtlAllocation);
	});
}
}
