
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
    void flushQueue() override;

    [[nodiscard]] ID3D12CommandQueue* getNativeCommandQueue() const;
    [[nodiscard]] ID3D12CommandAllocator* getNativeCommandAllocator() const;

private:
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;

    Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
    UINT64 mCurrentFence = 0;
};
}