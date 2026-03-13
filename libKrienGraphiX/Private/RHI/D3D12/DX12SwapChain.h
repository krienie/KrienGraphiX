
#pragma once

#include "Private/RHI/RHISwapChain.h"

#include "Private/RHI/RHIUtils.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl\client.h>

#include <vector>

namespace kgx::RHI
{
class DX12ResourceView;
class DX12Texture2D;

class DX12SwapChain : public RHISwapChain
{
public:
	DX12SwapChain(UINT width, UINT height);
	~DX12SwapChain() override = default;

	bool create(RHICommandQueue* commandQueue, SDL_Window* window, unsigned int bufferCount, RHIPixelFormat pixelFormat) override;

	[[nodiscard]] RHIResourceView* getCurrentBufferView() override;
	
	void present() override;

private:
	void checkTearingSupport();

	Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	std::vector<std::shared_ptr<DX12Texture2D>> mBuffers;
	std::vector<std::shared_ptr<DX12ResourceView>> mBufferViews;

	UINT mWidth;
	UINT mHeight;
	bool mTearingSupport;
};

DEFINE_RESOURCE_CAST(DX12SwapChain, RHISwapChain)
}
