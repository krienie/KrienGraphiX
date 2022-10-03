
#pragma once

#include <memory>
#include <vector>

#include "RHIResourceView.h"

namespace kgx::RHI
{
class RHIGraphicsDevice;

class RHIResource
{
public:
    enum CreationFlags
    {
        None = 0,
        RenderTargetable = 1 << 1,
        UnorderedAccess = 1 << 2,
        ShaderResource = 1 << 3,
        DepthStencil = 1 << 4
    };

    explicit RHIResource(CreationFlags flags);
    virtual ~RHIResource() = default;

    [[nodiscard]] CreationFlags getCreationFlags() const { return mCreationFlags; }

    std::shared_ptr<RHIResourceView> getResourceViewByType(RHIResourceView::ViewType viewType);
    void addResourceView(const std::shared_ptr<RHIResourceView>& resourceView);

private:
    CreationFlags mCreationFlags;
    std::vector<std::shared_ptr<RHIResourceView>> mResourceViews;
};

}
