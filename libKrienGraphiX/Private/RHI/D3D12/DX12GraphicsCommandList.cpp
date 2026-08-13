
#include "DX12GraphicsCommandList.h"

#include <cassert>

#include "DX12Buffer.h"
#include "DX12CommandAllocator.h"
#include "DX12GraphicsDevice.h"
#include "DX12GraphicsPipelineState.h"
#include "DX12RenderHardwareInterface.h"
#include "DX12ResourceView.h"
#include "Private/Rendering/KGXMeshRenderObject.h"

namespace
{
D3D12_CLEAR_FLAGS toDxClearFlags(kgx::RHI::DepthStencilFlags flags)
{
	using DepthStencilFlags = kgx::RHI::DepthStencilFlags;

	assert(flags & DepthStencilFlags::DepthClear | flags & DepthStencilFlags::StencilClear);

	D3D12_CLEAR_FLAGS dxClearFlags = flags & DepthStencilFlags::DepthClear ? D3D12_CLEAR_FLAG_DEPTH : static_cast<D3D12_CLEAR_FLAGS>(0);
	dxClearFlags |= flags & DepthStencilFlags::StencilClear ? D3D12_CLEAR_FLAG_STENCIL : static_cast<D3D12_CLEAR_FLAGS>(0);

	return dxClearFlags;
}
}

namespace kgx::RHI
{
DX12GraphicsCommandList::DX12GraphicsCommandList()
	: RHIGraphicsCommandList(), mCommandList(nullptr)
{
}

bool DX12GraphicsCommandList::create(RHIGraphicsPipelineState* initialState)
{
	ID3D12Device* nativeDevice = getDX12RHI()->getDX12Device()->getNativeDevice();

	// Create a temporary CommandAllocator here. We are going to immediately close the commandlist after this anyways, so it doesn't matter that this allocator is temporary.
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> tempAllocator;
	HRESULT res = nativeDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&tempAllocator));
	if (FAILED(res))
	{
		return false;
	}

	ID3D12PipelineState* nativeInitialState = nullptr;

	if (initialState != nullptr)
	{
		nativeInitialState = rcCast(initialState)->getPSO();
	}

	res = nativeDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, tempAllocator.Get(), nativeInitialState, IID_PPV_ARGS(&mCommandList));
	if (SUCCEEDED(res))
	{
		close();
	}

	return SUCCEEDED(res);
}

void DX12GraphicsCommandList::close()
{
	mCommandList->Close();
}

void DX12GraphicsCommandList::reset(RHICommandAllocator* allocator, RHIGraphicsPipelineState* initialState)
{
	ID3D12PipelineState* nativeInitialState = nullptr;

	if (initialState != nullptr)
	{
		DX12GraphicsPipelineState* dxPipelineState = rcCast(initialState);
		nativeInitialState = dxPipelineState->getPSO();
	}

	const DX12CommandAllocator* dxAllocator = rcCast(allocator);
	mCommandList->Reset(dxAllocator->getNativeAllocator(), nativeInitialState);
}

void DX12GraphicsCommandList::setPipelineState(RHIGraphicsPipelineState* pipelineState)
{
	DX12GraphicsPipelineState* dxPipelineState = rcCast(pipelineState);
	ID3D12PipelineState* nativePipelineState = dxPipelineState->getPSO();
	mCommandList->SetPipelineState(nativePipelineState);
	mCommandList->SetGraphicsRootSignature(dxPipelineState->getRootSignature());
}

void DX12GraphicsCommandList::setConstantBuffer(const RHIBuffer* constantBuffer, uint32_t bufferIndex)
{
	const DX12Buffer* dxConstantBuffer = rcCast(constantBuffer);
	mCommandList->SetGraphicsRootConstantBufferView(bufferIndex, dxConstantBuffer->getResource()->GetGPUVirtualAddress());
}

void DX12GraphicsCommandList::setViewport(const core::KGXViewport& viewport)
{
	const D3D12_VIEWPORT dxViewport =
	{
		static_cast<float>(viewport.topLeftX),
		static_cast<float>(viewport.topLeftY),
		static_cast<float>(viewport.width),
		static_cast<float>(viewport.height),
		viewport.minDepth,
		viewport.maxDepth
	};
	mCommandList->RSSetViewports(1u, &dxViewport);

	const D3D12_RECT scissorRect = { viewport.topLeftX, viewport.topLeftY, static_cast<long>(viewport.width), static_cast<long>(viewport.height) };
	mCommandList->RSSetScissorRects(1u, &scissorRect);
}

void DX12GraphicsCommandList::setRenderTargets(const std::vector<RHIResourceView*>& renderTargetViews, const RHIResourceView* depthStencilView)
{
	const DX12ResourceView* dxDsv = rcCast(depthStencilView);
	assert(dxDsv->getViewType() == RHIResourceView::Type::DSV);

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvCpuHandles(renderTargetViews.size());
	for (uint8_t i = 0u; i < static_cast<uint8_t>(renderTargetViews.size()); ++i)
	{
		DX12ResourceView* dxRtv = rcCast(renderTargetViews[i]);
		assert(dxRtv->getViewType() == RHIResourceView::Type::RTV);

		rtvCpuHandles[i] = dxRtv->getViewHandle();
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuHandle = dxDsv->getViewHandle();

	mCommandList->OMSetRenderTargets(static_cast<UINT>(rtvCpuHandles.size()), rtvCpuHandles.data(), false, &dsvCpuHandle);
}

void DX12GraphicsCommandList::clearDepthStencilView(RHIResourceView* dsv, DepthStencilFlags clearFlags, float depth, uint8_t stencil)
{
	DX12ResourceView* dxDsv = rcCast(dsv);
	assert(dxDsv->getViewType() == RHIResourceView::Type::DSV);
	
	mCommandList->ClearDepthStencilView(dxDsv->getViewHandle(), toDxClearFlags(clearFlags), depth, stencil, 0, nullptr);
}

void DX12GraphicsCommandList::clearRenderTargetView(RHIResourceView* rtv, const float colorRGBA[4])
{
	DX12ResourceView* dxRtv = rcCast(rtv);
	assert(dxRtv->getViewType() == RHIResourceView::Type::RTV);

	mCommandList->ClearRenderTargetView(dxRtv->getViewHandle(), colorRGBA, 0, nullptr);
}

void DX12GraphicsCommandList::drawMeshRenderObject(const rendering::KGXMeshRenderObject* renderObject)
{
	DX12Buffer* dxIndexBuffer = rcCast(renderObject->getIndexBuffer());
	DX12Buffer* dxVertexBuffer = rcCast(renderObject->getVertexBuffer());

	//TODO(KL): For now this is grouped together like this. Later will be packed into a drawpacket or something for easy handling
	//TODO(KL): Hard-coded to triangle list for now
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mCommandList->IASetIndexBuffer(dxIndexBuffer->getIndexBufferView());
	mCommandList->IASetVertexBuffers(0, 1, dxVertexBuffer->getVertexBufferView());
	mCommandList->DrawIndexedInstanced(static_cast<UINT>(renderObject->getNumIndices()), 1, 0, 0, 0);
}
}