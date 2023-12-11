
#pragma once

#include <memory>

#include "RHIResource.h"

namespace kgx::RHI
{
class RHIResourceView;

class RHIViewableResource : public RHIResource
{
public:
    RHIViewableResource(CreationFlags flags)
        : RHIResource(flags) {}
    ~RHIViewableResource() override = default;
};
}
