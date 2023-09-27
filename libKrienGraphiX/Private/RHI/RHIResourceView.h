
#pragma once

#include <memory>

namespace kgx::RHI
{
class RHIResource;

class RHIResourceView
{
public:
    enum ViewType
    {
        RTV = 0,
        DSV,
        CBV,
        SRV,
        UAV,
        NumTypes
    };

    enum DepthStencilFlags
    {
        DepthClear = 1,
        StencilClear = 2,

        DepthStencilClear = DepthClear | StencilClear
    };

    RHIResourceView(ViewType type, const std::shared_ptr<RHIResource>& viewedResource) : mViewType(type), mViewedResource(viewedResource) {}
    virtual ~RHIResourceView() = default;

    [[nodiscard]] ViewType getViewType() const { return mViewType; }
    [[nodiscard]] RHIResource* getViewedResource() const
    {
        if (const auto resource = mViewedResource.lock())
        {
            return resource.get();
        }

        return nullptr;
    }

private:
    ViewType mViewType;
    std::weak_ptr<RHIResource> mViewedResource;
};

}
