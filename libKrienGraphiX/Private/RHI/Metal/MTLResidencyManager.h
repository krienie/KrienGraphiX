
#pragma once

namespace kgx::RHI
{
class RHIResource;

class MTLResidencyManager final
{
public:
	MTLResidencyManager() = default;

	static void addGlobalResidency(const RHIResource& resource);
	static void removeGlobalResidency(const RHIResource& resource);
};
}
