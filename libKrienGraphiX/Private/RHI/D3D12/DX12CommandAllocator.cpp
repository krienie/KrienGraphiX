
#include "DX12CommandAllocator.h"

#include "DX12GraphicsDevice.h"
#include "DX12RenderHardwareInterface.h"

namespace kgx::RHI
{
DX12CommandAllocator::DX12CommandAllocator()
	: RHICommandAllocator()
{
	ID3D12Device* nativeDevice = getDX12RHI()->getDX12Device()->getNativeDevice();

	HRESULT res = nativeDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mDXCommandAllocator));
	if (FAILED(res))
	{
		mDXCommandAllocator = nullptr;
	}
}

ID3D12CommandAllocator* DX12CommandAllocator::getNativeAllocator() const
{
	return mDXCommandAllocator.Get();
}

void DX12CommandAllocator::reset()
{
	mDXCommandAllocator->Reset();
}
}
