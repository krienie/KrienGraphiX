
#include "RenderCore.h"

#ifdef WIN32
#include "Private/RHI/D3D12/DX12RenderHardwareInterface.h"
#endif

#include <cassert>
#include <iostream>

namespace kgx::core
{
RenderCore* RenderCore::mInst = nullptr;
int RenderCore::mRefCount = 0;

RenderCore* RenderCore::get()
{
    return mInst;
}

void RenderCore::startup()
{
    ++mRefCount;

    if ( !mInst )
    {
        mInst = new RenderCore();
    }
}

void RenderCore::shutdown()
{
    --mRefCount;

    if (mRefCount == 0)
    {
        delete mInst;
    }
}

RenderCore::RenderCore()
    : mRenderThread(), mRHI(nullptr), mGraphicsDevice(nullptr), mCommandList(nullptr)
{
#ifdef WIN32
    mRHI = std::make_unique<RHI::DX12RenderHardwareInterface>();
#else
    static_assert(false, "Only DirectX 12 (Windows 10 and up) is currently supported");
#endif

    assert(mRHI);

    mGraphicsDevice = mRHI->createGraphicsDevice();
    mCommandQueue   = mRHI->createCommandQueue(mGraphicsDevice.get());
    mCommandList    = mRHI->createGraphicsCommandList(mGraphicsDevice.get(), mCommandQueue.get(), nullptr);
}

RHI::RHIGraphicsDevice * RenderCore::getGraphicsDevicePtr() const
{
    return mGraphicsDevice.get();
}

RHI::RHICommandQueue * RenderCore::getGraphicsCommandQueuePtr() const
{
    return mCommandQueue.get();
}

RHI::RHIGraphicsCommandList* RenderCore::getGraphicsCommandListPtr() const
{
    return mCommandList.get();
}

RHI::RenderHardwareInterface * RenderCore::getRHI() const
{
    return mRHI.get();
}
}
