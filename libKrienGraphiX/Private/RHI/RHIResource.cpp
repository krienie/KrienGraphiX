
#include "RHIResource.h"

#include <cassert>

namespace kgx::RHI
{
RHIResource::RHIResource(CreationFlags flags)
    : mCreationFlags(flags), mResourceViews(static_cast<size_t>(RHIResourceView::ViewType::NumTypes))
{
}

std::shared_ptr<RHIResourceView> RHIResource::getResourceViewByType(RHIResourceView::ViewType viewType)
{
    return mResourceViews[static_cast<size_t>(viewType)];
}

void RHIResource::addResourceView(const std::shared_ptr<RHIResourceView>& resourceView)
{
    // TODO(KL): Support multiple views of the same type

    assert(resourceView);
    mResourceViews[static_cast<size_t>(resourceView->getViewType())] = resourceView;
}
}
