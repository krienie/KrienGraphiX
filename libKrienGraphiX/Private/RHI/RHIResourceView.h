
#pragma once

#include <memory>

namespace kgx::RHI
{
class RHIViewableResource;

/**
 * \brief Describes a view to a shader bindable resource that can be visible inside a shader.
 * Does not include Vertex and Index buffers.
 */
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
        //VertexBuffer,
        //IndexBuffer,
        NumTypes
    };

    RHIResourceView(ViewType type, const std::shared_ptr<RHIViewableResource>& viewedResource);
    virtual ~RHIResourceView() = default;

    [[nodiscard]] bool IsBufferView() const;
    [[nodiscard]] bool IsTextureView() const;

    [[nodiscard]] ViewType getViewType() const;
    [[nodiscard]] RHIViewableResource* getViewedResource() const;

private:
    ViewType mViewType;
    std::weak_ptr<RHIViewableResource> mViewedResource;
};

}
