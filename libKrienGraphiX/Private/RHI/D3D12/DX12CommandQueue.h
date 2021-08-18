
#pragma once

#include "Private/RHI/RHICommandQueue.h"

#include <d3d12.h>
#include <wrl\client.h>

namespace kgx::RHI
{
class DX12CommandQueue final : public RHICommandQueue
{
    public:
        DX12CommandQueue();
        ~DX12CommandQueue() override = default;

        bool init(RHIGraphicsDevice *device) override;
        void executeCommandList(RHIGraphicsCommandList * commandList) override;

        ID3D12CommandQueue * getNativeCommandQueue() const;

    private:
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
};
}