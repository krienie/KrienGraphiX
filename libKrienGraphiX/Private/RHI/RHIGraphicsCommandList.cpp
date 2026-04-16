
#include "RHIGraphicsCommandList.h"

#include "Private/Core/RenderCore.h"

namespace kgx::RHI
{
void RHIGraphicsCommandList::release()
{
	core::gRenderThread->getCommandListPoolPtr()->returnResource(this);
}
}
