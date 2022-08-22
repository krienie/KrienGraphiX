
#include "DX12GraphicsDevice.h"

using Microsoft::WRL::ComPtr;

namespace
{
// From https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Libraries/D3DX12Residency
// When rendering very quickly, it is possible for the renderer to get too far ahead of the library's worker thread.
// The MaxLatency parameter helps to limit how far ahead it can get. The value should essentially be the average
// NumberOfBufferedFrames * NumberOfCommandListSubmissionsPerFrame throughout the execution of your app.
constexpr int residencyManagerLatency = 6;

void GetHardwareAdapter(IDXGIFactory4 * pFactory, IDXGIAdapter1 ** ppAdapter, bool requestHighPerformanceAdapter)
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        const DXGI_GPU_PREFERENCE gpuPreference =
            requestHighPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED;

        for (UINT adapterIndex = 0;
             DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(adapterIndex, gpuPreference, IID_PPV_ARGS(&adapter));
             ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }
    else
    {
        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }
    
    *ppAdapter = adapter.Detach();
}
}

namespace kgx::RHI
{
DX12GraphicsDevice::DX12GraphicsDevice()
    : RHIGraphicsDevice(), mDevice(nullptr), mFactory(nullptr), mHardwareAdapter(nullptr), mResidencyManager()
{}

DX12GraphicsDevice::~DX12GraphicsDevice()
{
    //mResidencyManager.Destroy();
}

bool DX12GraphicsDevice::init()
{
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        } else
        {
            //TODO(KL): log warning
        }
    }
#endif

    HRESULT res = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&mFactory));
    if (FAILED(res))
    {
        return false;
    }

    GetHardwareAdapter(mFactory.Get(), &mHardwareAdapter, true);

    res = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mDevice));

    //TODO(KL): Use residency manager for something usefull...
    //mResidencyManager.Initialize(mDevice.Get(), 0, mHardwareAdapter.Get(), residencyManagerLatency);

    return SUCCEEDED(res);
}

ID3D12Device * DX12GraphicsDevice::getNativeDevice() const
{
    return mDevice.Get();
}

IDXGIFactory4 * DX12GraphicsDevice::getNativeFactory() const
{
    return mFactory.Get();
}

IDXGIAdapter1 * DX12GraphicsDevice::getNativeHardwareAdapter() const
{
    return mHardwareAdapter.Get();
}

D3DX12Residency::ResidencyManager * DX12GraphicsDevice::getResidencyManager()
{
    return &mResidencyManager;
}
}