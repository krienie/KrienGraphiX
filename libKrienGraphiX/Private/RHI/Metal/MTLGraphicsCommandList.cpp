
#include "MTLGraphicsCommandList.h"

#include <Metal/Metal.hpp>
#include <Metal/MTL4CommandBuffer.hpp>

#include "MTLBuffer.h"
#include "MTLCommandAllocator.h"
#include "MTLGraphicsPipelineState.h"
#include "MTLRenderHardwareInterface.h"
#include "MTLTexture2D.h"
#include "MTLShader.h"
#include "Metal/MTL4RenderPass.hpp"

#include "Private/Core/RenderCore.h"
#include "Private/RHI/RHIGraphicsCommandList.h"

namespace
{
MTL::ClearColor toMTLClearColor(const kgx::RHI::RHIClearValue& rhiClearValue)
{
	return MTL::ClearColor::Make(
		rhiClearValue.colorClear[0],
		rhiClearValue.colorClear[1],
		rhiClearValue.colorClear[2],
		rhiClearValue.colorClear[3]);
}
}

namespace kgx::RHI
{
MTLGraphicsCommandList::MTLGraphicsCommandList()
	: RHIGraphicsCommandList(), mPassContext()
{
}

bool MTLGraphicsCommandList::create(RHIGraphicsPipelineState* initialState)
{
	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice()->getNativeDevice();

	mCommandBuffer = NS::TransferPtr(mtlDevice->newCommandBuffer());
	if (!mCommandBuffer)
	{
		return false;
	}

	//TODO(KL): Pass in optional label to be used via constructor
	mCommandBuffer->setLabel(NS::String::string("RenderThreadCommandBuffer", NS::UTF8StringEncoding));

	return true;
}

void MTLGraphicsCommandList::close()
{
	if (mRecordedEncoderCommands.empty())
	{
		mCommandBuffer->endCommandBuffer();
		return;
	}
	
	if (mPassContext.mtlPipelineState && !mPassContext.topLevelBufferEntries.empty())
	{
		mPassContext.mtlPipelineState->setTopLevelBufferEntries(mPassContext.topLevelBufferEntries);
	}

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

	mPassContext = MTLPassCompilationContext();
	mPassDescriptor = reinterpret_cast<MTL4::RenderPassDescriptor*>(MTL::RenderPassDescriptor::renderPassDescriptor());
}

void MTLGraphicsCommandList::setPipelineState(RHIGraphicsPipelineState* pipelineState)
{
	mPassContext.mtlPipelineState = rcCast(pipelineState);

	recordEncoderCommand([=, this](MTL4::RenderCommandEncoder* encoder)
	{
		encoder->setRenderPipelineState(mPassContext.mtlPipelineState->getPSO());
	});

	const RHIGraphicsPipelineStateDescriptor& psoDescriptor = mPassContext.mtlPipelineState->getDescriptor();

	if (auto argumentTable = mPassContext.mtlPipelineState->getArgumentTable())
	{
		if (psoDescriptor.vs)
		{
			recordEncoderCommand([=](MTL4::RenderCommandEncoder* encoder)
			{
				encoder->setArgumentTable(argumentTable, MTL::RenderStageVertex);
			});
		}

		if (psoDescriptor.ps)
		{
			recordEncoderCommand([=](MTL4::RenderCommandEncoder* encoder)
			{
				encoder->setArgumentTable(argumentTable, MTL::RenderStageFragment);
			});
		}

		recordEncoderCommand([=](MTL4::RenderCommandEncoder* encoder)
		{
			encoder->setFrontFacingWinding(MTL::WindingClockwise);
		});
		recordEncoderCommand([=](MTL4::RenderCommandEncoder* encoder)
		{
			encoder->setCullMode(MTL::CullModeBack);
		});
	}
}

//TODO(KL): Abstract setting of a constant buffer away from this interface.
//Or perhaps create a layer above this where you set the shader together with it's constants.
void MTLGraphicsCommandList::setConstantBuffer(const RHIBuffer* constantBuffer)
{
	const MTLBuffer* mtlBuffer = rcCast(constantBuffer);

	IRDescriptorTableEntry entry;
	IRDescriptorTableSetBuffer(&entry, mtlBuffer->getGPUAddress(), 0);
	mPassContext.topLevelBufferEntries.push_back(entry);
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
	recordEncoderCommand([=](MTL4::RenderCommandEncoder* encoder)
	{
		encoder->setViewport(mtlViewport);
	});

	MTL::ScissorRect mtlScissorRect =
	{
		.x = 0, .y = 0, .width = viewport.width, .height = viewport.height
	};
	recordEncoderCommand([=](MTL4::RenderCommandEncoder* encoder)
	{
		encoder->setScissorRect(mtlScissorRect);
	});
}

void MTLGraphicsCommandList::setRenderTargets(const std::vector<RHIResourceView*>& renderTargetViews,
	const RHIResourceView* depthStencilView)
{
	if (renderTargetViews.empty())
	{
		assert(false);
		return;
	}

	mPassContext.renderTargets.reserve(renderTargetViews.size());
	for (RHIResourceView* renderTexture : renderTargetViews)
	{
		MTLTexture2D* mtlTexture = rcCast(renderTexture->getViewedResource());
		MTLPassTextureDescriptor& passTexture = mPassContext.usedTextures[mtlTexture];
		passTexture.storeAction = MTL::StoreActionStore;

		mPassContext.renderTargets.push_back(mtlTexture);
	}

	//MTLTexture2D* mtlDepthStencil = rcCast(depthStencilView->getViewedResource());
	//mPassCompilationContext.depthStencilTexture.texture = mtlDepthStencil;
	//mPassCompilationContext.depthStencilTexture.storeAction = MTL::StoreActionStore;

	//TODO(KL): Create DepthStencilState to be set in the renderencoder
}

void MTLGraphicsCommandList::clearDepthStencilView(RHIResourceView* dsv, DepthStencilFlags clearFlags, float depth,
	uint8_t stencil)
{
	if (!dsv->IsTextureView() || dsv->getViewType() != RHIResourceView::Type::DSV)
	{
		assert(false);
		return;
	}

	mPassContext.depthStencilTexture.loadAction = MTL::LoadActionClear;
	mPassContext.depthStencilTexture.clearValue.depthClear.depth = depth;
	mPassContext.depthStencilTexture.clearValue.depthClear.stencil = stencil;
	mPassContext.depthStencilTexture.dsClearFlags = clearFlags;
}

void MTLGraphicsCommandList::clearRenderTargetView(RHIResourceView* rtv, const float colorRGBA[4])
{
	if (!rtv->IsTextureView() || rtv->getViewType() != RHIResourceView::Type::RTV)
	{
		assert(false);
		return;
	}

	MTLTexture2D* mtlTexture = rcCast(rtv->getViewedResource());
	MTLPassTextureDescriptor& passTexture = mPassContext.usedTextures[mtlTexture];
	passTexture.texture = mtlTexture;
	passTexture.loadAction = MTL::LoadActionClear;

	std::memcpy(passTexture.clearValue.colorClear, colorRGBA, sizeof(passTexture.clearValue.colorClear));
}

void MTLGraphicsCommandList::drawMeshRenderObject(const rendering::KGXMeshRenderObject* renderObject)
{
	if (auto argumentTable = mPassContext.mtlPipelineState->getArgumentTable())
	{
		MTLBuffer* vertexBuffer = rcCast(renderObject->getVertexBuffer());
		MTLBuffer* indexBuffer = rcCast(renderObject->getIndexBuffer());

		argumentTable->setAddress(vertexBuffer->getGPUAddress(), kIRVertexBufferBindPoint);

		recordEncoderCommand([=](MTL4::RenderCommandEncoder* encoder)
		{
			encoder->drawIndexedPrimitives(
				MTL::PrimitiveTypeTriangle,
				renderObject->getNumIndices(),
				MTL::IndexTypeUInt16,
				indexBuffer->getGPUAddress(),
				indexBuffer->bufferSize()
			);
		});
	}
}

NS::SharedPtr<MTL4::RenderPassDescriptor> MTLGraphicsCommandList::compileToRenderPassDescriptor() const
{
	NS::SharedPtr<MTL4::RenderPassDescriptor> renderPassDescriptor = NS::TransferPtr(MTL4::RenderPassDescriptor::alloc()->init());

	assert(mPassContext.renderTargets.size() == mPassContext.usedTextures.size());

	for (int i = 0; i < mPassContext.renderTargets.size(); ++i)
	{
		// We are making the assumption here that the texture is always found, as we already did a loose assert above.
		auto passTextureDesc = mPassContext.usedTextures.find(mPassContext.renderTargets[i])->second;

		MTL::RenderPassColorAttachmentDescriptor* colorAttach = renderPassDescriptor->colorAttachments()->object(i);
		colorAttach->setTexture(passTextureDesc.texture->getNativeResource());
		colorAttach->setLoadAction(passTextureDesc.loadAction);
		colorAttach->setStoreAction(passTextureDesc.storeAction);
		colorAttach->setClearColor(toMTLClearColor(passTextureDesc.clearValue));
	}

	if (mPassContext.depthStencilTexture.texture)
	{
		const MTLPassTextureDescriptor& dsContext = mPassContext.depthStencilTexture;

		if (dsContext.dsClearFlags == (dsContext.dsClearFlags & DepthStencilFlags::DepthClear))
		{
			MTL::RenderPassDepthAttachmentDescriptor* depthAttach = renderPassDescriptor->depthAttachment();
			depthAttach->setClearDepth(dsContext.clearValue.depthClear.depth);
			depthAttach->setLoadAction(MTL::LoadActionClear);
			depthAttach->setStoreAction(MTL::StoreActionStore);
		}
		else
		{
			//TODO(KL): This isn't correct, but it will have to do for now
			renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionLoad);
			renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionDontCare);
		}

		if (dsContext.dsClearFlags == (dsContext.dsClearFlags & DepthStencilFlags::StencilClear))
		{
			MTL::RenderPassStencilAttachmentDescriptor* stencilAttach = renderPassDescriptor->stencilAttachment();
			stencilAttach->setClearStencil(dsContext.clearValue.depthClear.stencil);
			stencilAttach->setLoadAction(MTL::LoadActionClear);
			stencilAttach->setStoreAction(MTL::StoreActionStore);
		}
		else
		{
			//TODO(KL): This isn't correct, but it will have to do for now
			renderPassDescriptor->stencilAttachment()->setLoadAction(MTL::LoadActionLoad);
			renderPassDescriptor->stencilAttachment()->setStoreAction(MTL::StoreActionDontCare);
		}
	}

	renderPassDescriptor->setDefaultRasterSampleCount(1);

	return renderPassDescriptor;
}
}
