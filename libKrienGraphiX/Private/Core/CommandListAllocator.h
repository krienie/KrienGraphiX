
#pragma once

#include <memory>
#include <unordered_map>

#include "Private/RHI/RHIGraphicsCommandList.h"

namespace kgx::core
{
class CommandThread;

class CommandListAllocator final
{
public:
    CommandListAllocator(RHI::RHIGraphicsDevice* device, RHI::RHICommandQueue* commandQueue);
    ~CommandListAllocator();

    CommandListAllocator(const CommandListAllocator&) noexcept            = delete;
    CommandListAllocator(CommandListAllocator&&) noexcept                 = delete;
    CommandListAllocator& operator=(const CommandListAllocator&) noexcept = delete;
    CommandListAllocator& operator=(CommandListAllocator&&) noexcept      = delete;

    [[nodiscard]] RHI::RHIGraphicsCommandList* createGraphicsCommandList(RHI::RHIGraphicsPipelineState* initialState);
    void releaseGraphicsCommandList(RHI::RHIGraphicsCommandList* commandList);
    //[[nodiscard]] RHI::RHICommandQueue* getCommandQueuePtr() const;

private:
    struct CommandListAllocation
    {
        bool isFree = false;
        std::shared_ptr<RHI::RHIGraphicsCommandList> commandList;
    };

    RHI::RHIGraphicsDevice* mDevice = nullptr;
    RHI::RHICommandQueue* mCommandQueue = nullptr;

    //std::vector<CommandListAllocation> mCommandLists;
    std::unordered_map<RHI::RHIGraphicsCommandList*, CommandListAllocation> mCommandLists;
};
}
