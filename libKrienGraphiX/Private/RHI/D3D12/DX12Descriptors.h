
#pragma once

#include "d3dx12.h"
#include "Private/RHI/RHIDescriptors.h"

#include <wrl\client.h>

namespace kgx::RHI
{
struct DX12ConstantBufferDescriptor : RHIConstantBufferDescriptor
{
    Microsoft::WRL::ComPtr<ID3D12Heap> heap;
    UINT64 heapOffset;
    CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle;
};
}