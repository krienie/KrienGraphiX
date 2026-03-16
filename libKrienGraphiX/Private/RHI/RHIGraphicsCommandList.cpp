
#include "RHIGraphicsCommandList.h"

#include "Private/Core/RenderCore.h"

namespace kgx::RHI
{
void RHIGraphicsCommandList::release()
{
	core::RenderCore::get()->getRenderThreadPtr()->getCommandListPoolPtr()->release(this);
}
}
