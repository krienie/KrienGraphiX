
#include "MTLGraphicsCommandList.h"

#include <Metal/MTL4CommandBuffer.hpp>

#include "MTLCommandAllocator.h"
#include "MTLCommandQueue.h"
#include "MTLRenderHardwareInterface.h"
#include "MTLTexture2D.h"
#include "Metal/MTL4RenderPass.hpp"

#include "Private/Core/RenderCore.h"
#include "Private/RHI/RHIGraphicsCommandList.h"

namespace kgx::RHI
{
MTLGraphicsCommandList::MTLGraphicsCommandList()
	: RHIGraphicsCommandList()
{
}

bool MTLGraphicsCommandList::create(RHIGraphicsPipelineState* initialState)
{
	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice();

	mCommandBuffer = NS::TransferPtr(mtlDevice->newCommandBuffer());
	if (!mCommandBuffer)
	{
		return false;
	}

	return true;
}

void MTLGraphicsCommandList::close()
{
	NS::SharedPtr<MTL4::RenderPassDescriptor> renderPassDescriptor = compileToRenderPassDescriptor();

	MTL4::RenderCommandEncoder* renderEncoder = mCommandBuffer->renderCommandEncoder(renderPassDescriptor.get(), MTL4::RenderEncoderOptionNone);

	// Replay recorded encoder commands so they can be added to the just created encoder
	while (!mRecordedEncoderCommands.empty())
	{
		mRecordedEncoderCommands.front()(renderEncoder);
		mRecordedEncoderCommands.pop();
	}

	renderEncoder->endEncoding();

	mCommandBuffer->endCommandBuffer();
}

void MTLGraphicsCommandList::reset(RHICommandAllocator* allocator, RHIGraphicsPipelineState* initialState)
{
	//TODO(KL): Do something with initialState

	const MTLCommandAllocator* mtlAllocator = rcCast(allocator);
	mCommandBuffer->beginCommandBuffer(mtlAllocator->getNativeAllocator());

	mPassCompilationContext = MTLPassCompilationContext();
	mPassDescriptor = reinterpret_cast<MTL4::RenderPassDescriptor*>(MTL::RenderPassDescriptor::renderPassDescriptor());
}

void MTLGraphicsCommandList::setPipelineState(RHIGraphicsPipelineState* pipelineState)
{
	//TODO(KL): Implement
	assert(false);
}

void MTLGraphicsCommandList::setConstantBuffer(const RHIBuffer* constantBuffer)
{
	//TODO(KL): Implement
	assert(false);
}

void MTLGraphicsCommandList::setViewport(const core::KGXViewport& viewport)
{
	MTL::Viewport mtlViewport =
	{
		.originX = static_cast<double>(viewport.topLeftX),
		.originY = static_cast<double>(viewport.topLeftY),
		.width = static_cast<double>(viewport.width),
		.height = static_cast<double>(viewport.height),
		.znear = static_cast<double>(viewport.minDepth),
		.zfar = static_cast<double>(viewport.maxDepth),
	};
	recordEncodercommand(&MTL4::RenderCommandEncoder::setViewport, mtlViewport);
}

void MTLGraphicsCommandList::setRenderTargets(const std::vector<RHIResourceView*>& renderTargetViews,
	const RHIResourceView* depthStencilView)
{
	if (renderTargetViews.empty())
	{
		assert(false);
		return;
	}

	mPassCompilationContext.renderTargets.reserve(renderTargetViews.size());
	for (RHIResourceView* renderTexture : renderTargetViews)
	{
		MTLTexture2D* mtlTexture = rcCast(renderTexture->getViewedResource());
		MTLPassTextureDescriptor& passTexture = mPassCompilationContext.usedTextures[mtlTexture];
		passTexture.storeAction = MTL::StoreActionStore;

		mPassCompilationContext.renderTargets.push_back(mtlTexture);
	}

	//MTLTexture2D* mtlDepthStencil = rcCast(depthStencilView->getViewedResource());
	//mPassCompilationContext.depthStencilTexture.texture = mtlDepthStencil;
	//mPassCompilationContext.depthStencilTexture.storeAction = MTL::StoreActionStore;
}

void MTLGraphicsCommandList::clearDepthStencilView(RHIResourceView* dsv, DepthStencilFlags clearFlags, float depth,
	uint8_t stencil)
{
	//TODO(KL): Implement
	assert(false);

	if (!dsv->IsTextureView() || dsv->getViewType() != RHIResourceView::Type::DSV)
	{
		assert(false);
		return;
	}

	//mPassDescriptor->depthAttachment()
	//mPassDescriptor->stencilAttachment()

	//MTLTexture2D* mtlTexture = rcCast(dsv->getViewedResource());
	//mPassCompilationContext.depthStencilTexture.clearColor.
}

void MTLGraphicsCommandList::clearRenderTargetView(RHIResourceView* rtv, const float colorRGBA[4])
{
	if (!rtv->IsTextureView() || rtv->getViewType() != RHIResourceView::Type::RTV)
	{
		assert(false);
		return;
	}

	MTLTexture2D* mtlTexture = rcCast(rtv->getViewedResource());
	MTLPassTextureDescriptor& passTexture = mPassCompilationContext.usedTextures[mtlTexture];
	passTexture.texture = mtlTexture;
	passTexture.clearColor = MTL::ClearColor::Make(colorRGBA[0], colorRGBA[1], colorRGBA[2], colorRGBA[3]);
	passTexture.loadAction = MTL::LoadActionClear;
}

void MTLGraphicsCommandList::drawMeshRenderObject(const rendering::KGXMeshRenderObject* renderObject)
{
	//TODO(KL): Implement
	assert(false);
}

NS::SharedPtr<MTL4::RenderPassDescriptor> MTLGraphicsCommandList::compileToRenderPassDescriptor() const
{
	NS::SharedPtr<MTL4::RenderPassDescriptor> renderPassDescriptor = NS::TransferPtr(MTL4::RenderPassDescriptor::alloc()->init());

	assert(mPassCompilationContext.renderTargets.size() == mPassCompilationContext.usedTextures.size());

	for (int i = 0; i < mPassCompilationContext.renderTargets.size(); ++i)
	{
		// We are making the assumption here that the texture is always found, as we already did a loose assert above.
		auto passTextureDesc = mPassCompilationContext.usedTextures.find(mPassCompilationContext.renderTargets[i])->second;

		MTL::RenderPassColorAttachmentDescriptor* colorAttach = renderPassDescriptor->colorAttachments()->object(i);
		colorAttach->setTexture(passTextureDesc.texture->getNativeResource());
		colorAttach->setLoadAction(passTextureDesc.loadAction);
		colorAttach->setStoreAction(passTextureDesc.storeAction);
		colorAttach->setClearColor(passTextureDesc.clearColor);
	}

	//TODO(KL): add DepthStencil and the rest

	return renderPassDescriptor;
}
}
