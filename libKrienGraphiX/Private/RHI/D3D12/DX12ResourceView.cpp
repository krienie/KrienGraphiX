#include "DX12ResourceView.h"

#include <cassert>

#include "d3dx12.h"
#include "DX12ConstantBuffer.h"
#include "DX12GraphicsDevice.h"
#include "DX12Resource.h"
#include "Private/Core/RenderCore.h"

namespace
{
D3D12_DESCRIPTOR_HEAP_TYPE toDescriptorHeapType(kgx::RHI::DX12ResourceView::ViewType viewType)
{
    using ViewType = kgx::RHI::DX12ResourceView::ViewType;

    switch (viewType)
    {
        case ViewType::RTV:
            return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        case ViewType::DSV:
            return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        case ViewType::CBV:
        case ViewType::SRV:
        case ViewType::UAV:
            return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        default:
            break;
    }

    return D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
}
}

namespace kgx::RHI
{
DX12ResourceView::DX12ResourceView(ViewType type, const std::shared_ptr<RHIResource>& viewedResource, bool isShaderVisible)
    : RHIResourceView(type, viewedResource)
{
    createView(isShaderVisible);
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12ResourceView::getViewHandle() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(mDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), mDescriptorOffset);
}

void DX12ResourceView::createView(bool isShaderVisible)
{
    auto* dxDevice = dynamic_cast<DX12GraphicsDevice*>(core::RenderCore::get()->getRenderThreadPtr()->getGraphicsDevicePtr());
    auto* nativeDevice = dxDevice->getNativeDevice();

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
    heapDesc.NumDescriptors = 1u;
    heapDesc.Type = toDescriptorHeapType(getViewType());
    heapDesc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 0;
    nativeDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(mDescriptorHeap.GetAddressOf()));

    auto* dxResource = dynamic_cast<DX12Resource*>(getViewedResource());
    if (!dxResource)
    {
        return;
    }

    auto* nativeResource = dxResource->getResource().Get();

    switch (getViewType())
    {
        case ViewType::RTV:
            nativeDevice->CreateRenderTargetView(nativeResource, nullptr, mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
            break;
        case ViewType::DSV:
            nativeDevice->CreateDepthStencilView(nativeResource, nullptr, mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
            break;
        case ViewType::CBV:
            {
                const D3D12_RESOURCE_DESC nativeResourceDesc = nativeResource->GetDesc();
                
                D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
                cbvDesc.BufferLocation = nativeResource->GetGPUVirtualAddress();
                cbvDesc.SizeInBytes = static_cast<UINT>(nativeResourceDesc.Width);
                nativeDevice->CreateConstantBufferView(&cbvDesc, mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
            }
            break;
        case ViewType::SRV:
            nativeDevice->CreateShaderResourceView(nativeResource, nullptr, mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
            break;
        case ViewType::UAV:
            nativeDevice->CreateUnorderedAccessView(nativeResource, nullptr, nullptr, mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
            break;
        default:
            break;
    }
}

}
