
#pragma once

namespace kgx::RHI
{
class MTLResidencyManager final
{
public:
	MTLResidencyManager() = default;

	void addGlobalResidency(const class RHIResource& resource);
};
}
