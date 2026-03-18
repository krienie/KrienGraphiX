
#pragma once

#include "Private/RHI/RHICommandQueue.h"

#include <d3d12.h>
#include <memory>
#include <wrl\client.h>

#include "Private/RHI/RHIFence.h"
#include "Private/RHI/RHIUtils.h"

namespace kgx::RHI
{
class DX12CommandQueue final : public RHICommandQueue
{
public:
	DX12CommandQueue();
	~DX12CommandQueue() override = default;

	bool create() override;
	void executeCommandList(RHIGraphicsCommandList* commandList) override;

	[[nodiscard]] ID3D12CommandQueue* getNativeCommandQueue() const;

private:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
};

DEFINE_RESOURCE_CAST(DX12CommandQueue, RHICommandQueue)
}
