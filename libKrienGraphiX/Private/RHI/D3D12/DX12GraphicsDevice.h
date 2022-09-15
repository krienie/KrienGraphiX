
#pragma once

#include "Private/RHI/RHIGraphicsDevice.h"
#include "Private/RHI/D3D12/d3dx12Residency.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl\client.h>

namespace kgx::RHI
{
class DX12GraphicsDevice : public RHIGraphicsDevice
{
    public:
        DX12GraphicsDevice();
        ~DX12GraphicsDevice() override;

        bool init() override;

        [[nodiscard]]
        ID3D12Device* getNativeDevice() const;

        [[nodiscard]]
        IDXGIFactory4* getNativeFactory() const;

        [[nodiscard]]
        IDXGIAdapter1* getNativeHardwareAdapter() const;

        [[nodiscard]]
        D3DX12Residency::ResidencyManager* getResidencyManager();

    private:
        Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
        Microsoft::WRL::ComPtr<IDXGIFactory4> mFactory;
        Microsoft::WRL::ComPtr<IDXGIAdapter1> mHardwareAdapter;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
        D3DX12Residency::ResidencyManager mResidencyManager;
};
}