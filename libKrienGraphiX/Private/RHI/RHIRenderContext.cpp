
#include "RHIRenderContext.h"

#include "Private/Core/RenderThread.h"

namespace kgx::RHI
{
RHIRenderContext::RHIRenderContext(core::ResourcePool<RHIRenderContext>& parentPool)
	: PooledType(parentPool)
{
}
}
