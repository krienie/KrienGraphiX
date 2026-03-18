
#pragma once

#include "Private/RHI/RHIResourceView.h"

namespace kgx::RHI
{
class MTLTextureView : public RHIResourceView
{
public:
	MTLTextureView(Type type, const std::shared_ptr<RHIViewableResource>& viewedResource);
};
}

