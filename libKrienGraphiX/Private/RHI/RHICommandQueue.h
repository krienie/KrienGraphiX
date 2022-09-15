
#pragma once

namespace kgx::RHI
{
class RHIGraphicsDevice;
class RHIGraphicsCommandList;

class RHICommandQueue
{
    public:
        virtual ~RHICommandQueue() = default;

        virtual bool init(RHIGraphicsDevice *device) = 0;

        virtual void executeCommandList(RHIGraphicsCommandList* commandList) = 0;
        virtual void flushQueue() = 0;

    //TODO(KL): add interface to add commands to the queue
};
}