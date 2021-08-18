
#pragma once

#include <memory>

#include "Private/RHI/RHIGraphicsCommandList.h"

namespace kgx::RenderMethods
{
class RenderPass
{
    public:
        RenderPass();
        virtual ~RenderPass() = default;

        virtual void preRenderSetup() = 0;
        virtual void update() = 0;
        virtual void render() = 0;

    private:
        std::unique_ptr<RHI::RHIGraphicsCommandList> mCommandList;
};
}
