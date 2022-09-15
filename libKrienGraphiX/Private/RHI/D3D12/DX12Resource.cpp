#include "DX12Resource.h"

#include "d3dx12.h"
#include "DX12GraphicsCommandList.h"

namespace kgx::RHI
{
DX12Resource::DX12Resource(Microsoft::WRL::ComPtr<ID3D12Resource> resourcePtr, Microsoft::WRL::ComPtr<ID3D12Heap> heapPtr, D3D12_RESOURCE_STATES initialState)
    : mCurrentState(initialState), mResource(std::move(resourcePtr)), mHeap(std::move(heapPtr))
{
}

void DX12Resource::transitionToState(DX12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState)
{
    commandList->getCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mResource.Get(), mCurrentState, newState));

    //TODO(KL): Maybe call this pending state as the cmdlist hasn't been executed yet?
    //TODO(KL): Or find a way to properly sync this to the cmdlist (events/fences?)
    mCurrentState = newState;
}
}
