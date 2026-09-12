
#include "MTLResidencyManager.h"

#include "MTLCommandQueue.h"
#include "MTLPlatform.h"
#include "Private/Core/RenderThread.h"

namespace kgx::RHI
{
void MTLResidencyManager::addGlobalResidency(const RHIResource& resource)
{
	core::gRenderThread->enqueueCommand([&resource]()
	{
		auto* mtlPlatform = static_cast<MTLPlatform*>(core::gRenderThread->getRHIPlatformPtr());
		MTLCommandQueue& mtlCommandQueue = mtlPlatform->getCommandQueue();
		mtlCommandQueue.addGlobalResidency(static_cast<MTL::Allocation*>(resource.getNativeResource()));
	});
}
}
