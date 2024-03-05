
#include "RHIGraphicsCommandList.h"

#include "Private/Core/CommandListAllocator.h"

namespace kgx::RHI
{
RHIGraphicsCommandList::RHIGraphicsCommandList(core::CommandListAllocator& allocator)
    : mAllocator(&allocator)
{}

RHIGraphicsCommandList::~RHIGraphicsCommandList()
{
    close();
    release();
}

void RHIGraphicsCommandList::release()
{
    mAllocator->releaseGraphicsCommandList(this);
}

void RHIGraphicsCommandList::close()
{
    if (!mIsClosed)
    {
        closeInternal();
        mIsClosed = true;
    }
}
}
