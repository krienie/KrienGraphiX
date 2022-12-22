#include "DX12Resource.h"

#include "d3dx12.h"
#include "DX12GraphicsCommandList.h"

namespace kgx::RHI
{
DX12Resource::DX12Resource(Microsoft::WRL::ComPtr<ID3D12Resource> resourcePtr, Microsoft::WRL::ComPtr<ID3D12Heap> heapPtr, D3D12_RESOURCE_STATES initialState)
    : mResource(std::move(resourcePtr)), mHeap(std::move(heapPtr)), mCurrentState(initialState)
{
}

void DX12Resource::transitionToState(DX12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState)
{
    const auto transition = CD3DX12_RESOURCE_BARRIER::Transition(mResource.Get(), mCurrentState, newState);
    commandList->getCommandList()->ResourceBarrier(1, &transition);

    //TODO(KL): Maybe call this pending state as the cmdlist hasn't been executed yet?
    //TODO(KL): Or find a way to properly sync this to the cmdlist (events/fences?)
    mCurrentState = newState;
}

D3D12_RESOURCE_FLAGS toDx12ResourceFlags(RHIResource::CreationFlags flags)
{
    using CreationFlags = RHIResource::CreationFlags;

    D3D12_RESOURCE_FLAGS outResourceFlags = D3D12_RESOURCE_FLAG_NONE;
    outResourceFlags |= flags & CreationFlags::RenderTargetable ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE;
    outResourceFlags |= flags & CreationFlags::UnorderedAccess ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;
    outResourceFlags |= flags & CreationFlags::ShaderResource ? D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE : D3D12_RESOURCE_FLAG_NONE;
    outResourceFlags |= flags & CreationFlags::DepthStencil ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE;

    return outResourceFlags;
}
}
