
#pragma once

#include <functional>
#include <memory>

#include "CommandListAllocator.h"
#include "Private/Rendering/KGXShaderCache.h"

#include "Private/RHI/RHIGraphicsCommandList.h"
#include "Private/RHI/RHIGraphicsDevice.h"
#include "Private/RHI/RHISwapChain.h"

namespace kgx::core
{
class CommandThread;

class RenderThread final
{
public:
    using RenderCommand = std::function<void()>;

    RenderThread();
    ~RenderThread();

    RenderThread(const RenderThread&) noexcept            = delete;
    RenderThread(RenderThread&&) noexcept                 = delete;
    RenderThread& operator=(const RenderThread&) noexcept = delete;
    RenderThread& operator=(RenderThread&&) noexcept      = delete;

    [[nodiscard]] RHI::RHIGraphicsDevice* getGraphicsDevicePtr() const;
    [[nodiscard]] RHI::RHICommandQueue* getCommandQueuePtr() const;
    [[nodiscard]] RHI::RHIGraphicsCommandList* getGraphicsCommandListPtr() const;

    void enqueueCommand(RenderCommand cmd) const;
    void flush() const;

private:
    std::unique_ptr<CommandThread> mCommandThread;

    std::unique_ptr<RHI::RHIGraphicsDevice> mGraphicsDevice;
    std::unique_ptr<RHI::RHICommandQueue> mCommandQueue;

    std::unique_ptr<CommandListAllocator> mCommandListAllocator;
    //std::unique_ptr<RHI::RHIGraphicsCommandList> mCommandList;

    std::unique_ptr<rendering::KGXShaderCache> mShaderCache;
};
}
