
#include "MTLCommandAllocator.h"

#include "MTLRenderHardwareInterface.h"

namespace kgx::RHI
{
MTLCommandAllocator::MTLCommandAllocator()
{
	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice()->getNativeDevice();

	mCommandAllocator = NS::TransferPtr(mtlDevice->newCommandAllocator());
}

MTL4::CommandAllocator* MTLCommandAllocator::getNativeAllocator() const
{
	return mCommandAllocator.get();
}

void MTLCommandAllocator::reset()
{
	mCommandAllocator->reset();
}
}
