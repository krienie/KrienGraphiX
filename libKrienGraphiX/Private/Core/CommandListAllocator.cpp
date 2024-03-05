
#include "CommandListAllocator.h"

#include <cassert>

#include "Private/RHI/RenderHardwareInterface.h"

namespace kgx::core
{
CommandListAllocator::CommandListAllocator(RHI::RHIGraphicsDevice* device, RHI::RHICommandQueue* commandQueue)
    : mDevice(device), mCommandQueue(commandQueue)
{
    //mCommandQueue   = RHI::PlatformRHI->createCommandQueue(mGraphicsDevice.get());
    //mCommandList    = RHI::PlatformRHI->createGraphicsCommandList(mGraphicsDevice.get(), mCommandQueue.get(), nullptr);
}

CommandListAllocator::~CommandListAllocator()
{
    //mCommandThread.reset();
    //RHI::PlatformRHI.reset();
}

RHI::RHIGraphicsCommandList* CommandListAllocator::createGraphicsCommandList(RHI::RHIGraphicsPipelineState* initialState)
{
    // For now I just loop through the entire list to get the next one that is open. If this causes too much of a performance hit I will think of a different way to handle this
    //for (int i = 0; i < mCommandLists.size(); ++i)
    for (const auto& [_, commandListAllocation] : mCommandLists)
    {
        if (commandListAllocation.isFree)
        {
            return commandListAllocation.commandList.get();
        }
    }

    const std::shared_ptr<RHI::RHIGraphicsCommandList> newCommandList = RHI::PlatformRHI->createGraphicsCommandList(this, mDevice, mCommandQueue, initialState);
    CommandListAllocation newAllocation
    {
        .isFree = false,
        .commandList = newCommandList
    };

    mCommandLists.insert(std::make_pair(newCommandList.get(), std::move(newAllocation)));

    return newCommandList.get();
}

void CommandListAllocator::releaseGraphicsCommandList(RHI::RHIGraphicsCommandList* commandList)
{
    if (auto it = mCommandLists.find(commandList); it != mCommandLists.end())
    {
        it->second.isFree = true;
    }
}
}
