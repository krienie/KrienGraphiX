
#include "DX12GraphicsCommandList.h"

#include <cassert>

#include "DX12Buffer.h"
#include "DX12CommandQueue.h"
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
DX12GraphicsCommandList::DX12GraphicsCommandList(core::CommandListAllocator* allocator)
	: RHIGraphicsCommandList(*allocator), mCommandAllocator(nullptr), mCommandList(nullptr)
{
}

bool DX12GraphicsCommandList::create(RHIGraphicsPipelineState* initialState)
{
	ID3D12Device* nativeDevice = getDX12RHI()->getDX12Device()->getNativeDevice();

	HRESULT res = nativeDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
	if (FAILED(res))
	{
		return false;
	}

	ID3D12PipelineState* nativeInitialState = nullptr;

	if (initialState != nullptr)
	{
		nativeInitialState = dxCast(initialState)->getPSO();
	}
	
	res = nativeDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nativeInitialState, IID_PPV_ARGS(&mCommandList));
	if (SUCCEEDED(res))
	{
	    // Start off closed, because the first thing we do when rendering is to close the commandallocator that was used in the previous frame.
	    mCommandList->Close();
	}

	return SUCCEEDED(res);
}

void DX12GraphicsCommandList::close()
{
	mCommandList->Close();
}

void DX12GraphicsCommandList::reset(RHIGraphicsPipelineState* initialState)
{
	ID3D12PipelineState* nativeInitialState = nullptr;

	if (initialState != nullptr)
	{
		DX12GraphicsPipelineState* dxPipelineState = dxCast(initialState);
		nativeInitialState = dxPipelineState->getPSO();
	}

	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.Get(), nativeInitialState);
}

void DX12GraphicsCommandList::setPipelineState(RHIGraphicsPipelineState* pipelineState)
{
	DX12GraphicsPipelineState* dxPipelineState = dxCast(pipelineState);
	ID3D12PipelineState* nativePipelineState = dxPipelineState->getPSO();
	mCommandList->SetPipelineState(nativePipelineState);
	mCommandList->SetGraphicsRootSignature(dxPipelineState->getRootSignature());

	//TODO(KL): Temporary like this
	const RHI::RHIGraphicsPipelineStateDescriptor& psoDesk = dxPipelineState->getDescriptor();
	std::vector<const RHIBuffer*> constBuffs = psoDesk.ps->getConstantBufferPtrs();

	if (constBuffs.size() > 0)
	{
		const DX12Buffer* firstCB = dxCast(constBuffs[0]);
		
		mCommandList->SetGraphicsRootConstantBufferView(0, firstCB->getResource()->GetGPUVirtualAddress());
	}
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
	const DX12ResourceView* dxDsv = dxCast(depthStencilView);
	assert(dxDsv->getViewType() == RHIResourceView::Type::DSV);

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvCpuHandles(renderTargetViews.size());
	for (uint8_t i = 0u; i < static_cast<uint8_t>(renderTargetViews.size()); ++i)
	{
		DX12ResourceView* dxRtv = dxCast(renderTargetViews[i]);
		assert(dxRtv->getViewType() == RHIResourceView::Type::RTV);

		rtvCpuHandles[i] = dxRtv->getViewHandle();
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuHandle = dxDsv->getViewHandle();

	mCommandList->OMSetRenderTargets(static_cast<UINT>(rtvCpuHandles.size()), rtvCpuHandles.data(), false, &dsvCpuHandle);
}

void DX12GraphicsCommandList::clearDepthStencilView(RHIResourceView* dsv, DepthStencilFlags clearFlags, float depth, uint8_t stencil)
{
	DX12ResourceView* dxDsv = dxCast(dsv);
	assert(dxDsv->getViewType() == RHIResourceView::Type::DSV);
	
	mCommandList->ClearDepthStencilView(dxDsv->getViewHandle(), toDxClearFlags(clearFlags), depth, stencil, 0, nullptr);
}

void DX12GraphicsCommandList::clearRenderTargetView(RHIResourceView* rtv, const float colorRGBA[4])
{
	DX12ResourceView* dxRtv = dxCast(rtv);
	assert(dxRtv->getViewType() == RHIResourceView::Type::RTV);

	mCommandList->ClearRenderTargetView(dxRtv->getViewHandle(), colorRGBA, 0, nullptr);
}

void DX12GraphicsCommandList::drawMeshRenderObject(const rendering::KGXMeshRenderObject* renderObject)
{
	DX12Buffer* dxIndexBuffer = dxCast(renderObject->getIndexBuffer());
	DX12Buffer* dxVertexBuffer = dxCast(renderObject->getVertexBuffer());

	//TODO(KL): For now this is grouped together like this. Later will be packed into a drawpacket or something for easy handling
	//TODO(KL): Hard-coded to triangle list for now
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mCommandList->IASetIndexBuffer(dxIndexBuffer->getIndexBufferView());
	mCommandList->IASetVertexBuffers(0, 1, dxVertexBuffer->getVertexBufferView());
	mCommandList->DrawInstanced(static_cast<UINT>(renderObject->getNumVertices()), 1, 0, 0);
}
} // namespace kgx::RHI