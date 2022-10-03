
#pragma once

#include "d3dx12.h"

#include "Private/RHI/RHIDescriptors.h"
#include "Private/RHI/RHIPixelFormat.h"

#include <wrl\client.h>

#include "DX12PixelFormat.h"

namespace kgx::RHI
{
struct DX12ConstantBufferDescriptor : RHIConstantBufferDescriptor
{
    Microsoft::WRL::ComPtr<ID3D12Heap> heap;
    UINT64 heapOffset;
    CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle;
};

struct DX12Texture2DDescriptor : RHITexture2DDescriptor
{
    Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
    int heapOffset;
    D3D12_RESOURCE_STATES initialState;
};

inline D3D12_CLEAR_VALUE toDx12ClearValue(RHIPixelFormat pixelFormat, const RHIClearValue &clearValue)
{
    D3D12_CLEAR_VALUE outClearValue;
    outClearValue.Format = toDxgiPixelFormat(pixelFormat);
    std::memcpy(outClearValue.Color, clearValue.colorClear, sizeof(clearValue.colorClear));

    return outClearValue;
}

inline D3D12_CLEAR_VALUE getDefaultColorClearValue(RHIPixelFormat pixelFormat)
{
    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = toDxgiPixelFormat(pixelFormat);
    std::memset(clearValue.Color, 0, sizeof(clearValue.Color));

    return clearValue;
}

inline D3D12_CLEAR_VALUE getDefaultDepthClearValue(RHIPixelFormat pixelFormat)
{
    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = toDxgiPixelFormat(pixelFormat);
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0u;

    return clearValue;
}
}
