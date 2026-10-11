#pragma once

#include <memory>
#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTLDepthStencil.hpp>

#include "Private/Rendering/KGXRenderPass.h"

namespace kgx::RHI
{
class MTLDepthStencilManager final
{
public:
	MTLDepthStencilManager();

	[[nodiscard]] MTL::DepthStencilState* getMTLDepthStencilState(const rendering::DepthStencilBinding& depthStencilBinding);

private:
	MTL::DepthStencilState* addDepthStencilState(const rendering::DepthStencilBinding& depthStencilBinding);

	std::unordered_map<XXH64_hash_t, NS::SharedPtr<MTL::DepthStencilState>> mDepthStencilStateCache;
};
}


