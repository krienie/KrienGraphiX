
#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "Private/RHI/RHIFence.h"

namespace kgx::core
{
class RenderThread;
}

namespace kgx::RHI
{
class DX12Fence : public RHIFence
{
public:
	DX12Fence();
	~DX12Fence() override;

	void sync() override;
	void queueSignal(uint64_t value) override;
	void waitForValue(uint64_t value) override;

private:
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;

	HANDLE mEventHandle = nullptr;
	uint64_t mFenceValue = 0;
};
}
