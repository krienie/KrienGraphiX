
#include "RHIResourceView.h"

#include "RHIViewableResource.h"

namespace kgx::RHI
{
RHIResourceView::RHIResourceView(ViewType type, const std::shared_ptr<RHIViewableResource>& viewedResource)
    : mViewType(type), mViewedResource(viewedResource)
{
}

bool RHIResourceView::IsBufferView() const
{
    const auto* viewedResource = getViewedResource();
    return viewedResource != nullptr ? viewedResource->IsBuffer() : false;
}

bool RHIResourceView::IsTextureView() const
{
    const auto* viewedResource = getViewedResource();
    return viewedResource != nullptr ? viewedResource->IsTexture() : false;
}

RHIResourceView::ViewType RHIResourceView::getViewType() const
{
    return mViewType;
}

RHIViewableResource* RHIResourceView::getViewedResource() const
{
    if (const auto resource = mViewedResource.lock())
    {
        return resource.get();
    }

    return nullptr;
}
}
