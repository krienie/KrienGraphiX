
#pragma once

#include <memory>

#include "Private/RHI/RHIGraphicsCommandList.h"

namespace kgx::rendering
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
