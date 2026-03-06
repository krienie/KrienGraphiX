
#include "MTLGraphicsDevice.h"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

namespace kgx::RHI
{
MTLGraphicsDevice::MTLGraphicsDevice()
{
}

MTLGraphicsDevice::~MTLGraphicsDevice()
{
}

bool MTLGraphicsDevice::create()
{
    mGraphicsDevice = NS::TransferPtr(MTL::CreateSystemDefaultDevice());

    return mGraphicsDevice.get() != nullptr;
}
}

