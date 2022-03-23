
#pragma once

#include <memory>

#include "Private/RHI/RHIGraphicsCommandList.h"

namespace kgx::Rendering
{
class KGXRenderer
{
    public:
        KGXRenderer();
        ~KGXRenderer() = default;

    private:
        std::unique_ptr<RHI::RHIGraphicsCommandList> mCommandList;
};
}
