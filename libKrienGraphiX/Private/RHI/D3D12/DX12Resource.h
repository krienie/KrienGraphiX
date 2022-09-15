
#pragma once

#include <d3d12.h>
#include <wrl\client.h>

namespace kgx::RHI
{
class DX12GraphicsCommandList;

class DX12Resource
{
public:
    DX12Resource(Microsoft::WRL::ComPtr<ID3D12Resource> resourcePtr, Microsoft::WRL::ComPtr<ID3D12Heap> heapPtr, D3D12_RESOURCE_STATES initialState);
    void transitionToState(DX12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState);

    [[nodiscard]] D3D12_RESOURCE_STATES getCurrentState() const { return mCurrentState; }
    [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> getResource() const { return mResource; }
    [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Heap> getHeap() const { return mHeap; }

private:
    D3D12_RESOURCE_STATES mCurrentState;

    Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
    Microsoft::WRL::ComPtr<ID3D12Heap> mHeap;
};
}
