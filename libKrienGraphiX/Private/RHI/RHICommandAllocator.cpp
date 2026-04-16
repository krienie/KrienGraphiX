
#include "RHICommandAllocator.h"

#include "Private/Core/RenderCore.h"

namespace kgx::RHI
{
void RHICommandAllocator::release()
{
	core::gRenderThread->getCommandAllocatorPoolPtr()->returnResource(this);
}
}
