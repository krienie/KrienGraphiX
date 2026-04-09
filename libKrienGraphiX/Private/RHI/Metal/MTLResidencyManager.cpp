
#include "MTLResidencyManager.h"

#include "MTLCommandQueue.h"
#include "MTLGraphicsCommandList.h"
#include "Private/Core/RenderThread.h"

namespace kgx::RHI
{
void MTLResidencyManager::addGlobalResidency(const RHIResource& resource)
{
	core::gRenderThread->enqueueCommand([&resource]()
	{
		MTLCommandQueue* mtlCommandQueue = rcCast(core::gRenderThread->getCommandQueuePtr());
		mtlCommandQueue->addGlobalResidency(static_cast<MTL::Allocation*>(resource.getNativeResource()));
	});
}
}
