
#pragma once

#include <memory>

#include "Foundation/NSSharedPtr.hpp"
#include "Metal/MTLDevice.hpp"

namespace kgx::RHI
{
class MTLGraphicsDevice
{
public:
    MTLGraphicsDevice();
    ~MTLGraphicsDevice();

    bool create();

private:
    NS::SharedPtr<MTL::Device> mGraphicsDevice;
};
}

