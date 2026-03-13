
#include "DX12SwapChain.h"

#include <SDL3/SDL.h>

#include "d3dx12.h"

#include "DX12CommandQueue.h"
#include "DX12GraphicsDevice.h"
#include "DX12PixelFormat.h"
#include "DX12Texture2D.h"
#include "d3dx12.h"
#include "DX12Descriptors.h"
#include "DX12RenderHardwareInterface.h"

using Microsoft::WRL::ComPtr;

namespace kgx::RHI
{
DX12SwapChain::DX12SwapChain(UINT width, UINT height)
	: RHISwapChain(), mSwapChain(nullptr), mRtvHeap(nullptr),
		mWidth(width), mHeight(height), mTearingSupport(false)
{
}

bool DX12SwapChain::create(RHICommandQueue* commandQueue, SDL_Window* window, unsigned int bufferCount, RHIPixelFormat pixelFormat)
{
	checkTearingSupport();

	DX12GraphicsDevice* dxDevice = getDX12RHI()->getDX12Device();
	IDXGIFactory4* nativeFactory = dxDevice->getNativeFactory();

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = bufferCount;
	swapChainDesc.Width = mWidth;
	swapChainDesc.Height = mHeight;
	swapChainDesc.Format = toDxgiPixelFormat(pixelFormat);
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Flags = mTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	DX12CommandQueue* dxCommandQueue = dxCast(commandQueue);
	ID3D12CommandQueue* nativeCommandQueue = dxCommandQueue->getNativeCommandQueue();

	ComPtr<IDXGISwapChain1> swapChain;

	const SDL_PropertiesID props = SDL_GetWindowProperties(mSDLWindow);
	kgx::WinHandle windowHandle = static_cast<kgx::WinHandle>(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));

	HRESULT res = nativeFactory->CreateSwapChainForHwnd(
		nativeCommandQueue,
		reinterpret_cast<HWND>(windowHandle),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	);

	if (FAILED(res))
	{
		return false;
	}

	swapChain.As(&mSwapChain);

	// Turn off fullscreen transitions
	res = nativeFactory->MakeWindowAssociation(reinterpret_cast<HWND>(windowHandle), DXGI_MWA_NO_ALT_ENTER);
	if ((FAILED(res)))
	{
		return false;
	}

	ID3D12Device* nativeDevice = dxDevice->getNativeDevice();

	// Create descriptor heap for the backbuffer RTV's
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = bufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	res = nativeDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap));
	if ((FAILED(res)))
	{
		return false;
	}

	//TODO(KL): maybe store this descriptor size in DX12ResourceView
	const UINT rtvDescriptorSize = nativeDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	std::vector<ComPtr<ID3D12Resource>> buffers(bufferCount);

	// Create render target resources
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0u; i < bufferCount; ++i)
	{
		res = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&buffers[i]));
		if ((FAILED(res)))
		{
			return false;
		}

		nativeDevice->CreateRenderTargetView(buffers[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, rtvDescriptorSize);

		DX12Texture2DDescriptor desc =
		{
			{
				{0, 0, 0, 0},
				pixelFormat,
				mWidth,
				mHeight
			},
			buffers[i],
			mRtvHeap,
			static_cast<int>(i),
			D3D12_RESOURCE_STATE_PRESENT
		};

		// Register the created D3D12 resources
		constexpr bool isShaderVisible = false;
		auto newBuffer = std::make_shared<DX12Texture2D>(desc);
		mBufferViews.push_back(std::make_shared<DX12ResourceView>(DX12ResourceView::Type::RTV, newBuffer, isShaderVisible));
		mBuffers.push_back(std::move(newBuffer));
	}

	return SUCCEEDED(res);
}

RHIResourceView* DX12SwapChain::getCurrentBufferView()
{
	return mBufferViews[mSwapChain->GetCurrentBackBufferIndex()].get();
}

void DX12SwapChain::present()
{
	const UINT presentFlags = (mTearingSupport /*&& mWindowedMode*/) ? DXGI_PRESENT_ALLOW_TEARING : 0;
	mSwapChain->Present(0, presentFlags);
}

void DX12SwapChain::checkTearingSupport()
{
	ComPtr<IDXGIFactory6> factory;
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	bool allowTearing = false;
	if (SUCCEEDED(hr))
	{
		hr = factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
	}

	mTearingSupport = SUCCEEDED(hr) && allowTearing;
}
}