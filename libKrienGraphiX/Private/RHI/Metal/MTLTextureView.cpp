
#include "MTLTextureView.h"

namespace kgx::RHI
{
MTLTextureView::MTLTextureView(Type type, const std::shared_ptr<RHIViewableResource>& viewedResource)
	: RHIResourceView(type, viewedResource)
{
}
}
