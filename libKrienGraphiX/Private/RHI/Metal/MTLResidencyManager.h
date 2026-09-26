
#pragma once

namespace MTL
{
class Allocation;
}

namespace kgx::RHI
{
class RHIResource;

class MTLResidencyManager final
{
public:
	MTLResidencyManager() = default;

	static void addGlobalResidency(const MTL::Allocation* allocation);
	static void removeGlobalResidency(const MTL::Allocation* allocation);
};
}
