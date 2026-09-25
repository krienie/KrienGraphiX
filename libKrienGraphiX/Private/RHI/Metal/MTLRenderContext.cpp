
#include "MTLRenderContext.h"

#include "MTLBuffer.h"
#include "MTLRenderHardwareInterface.h"
#include "MTLGraphicsDevice.h"
#include "MTLGraphicsPipelineState.h"
#include "MTLTexture2D.h"
#include "MTLUtils.h"
#include "Private/Core/RenderThread.h"
#include "Private/Rendering/KGXMeshRenderObject.h"
#include "Private/Rendering/KGXRenderPass.h"

namespace
{
MTL::LoadAction toMTLLoadAction(const kgx::rendering::TextureLoadAction& loadAction)
{
	using namespace kgx::rendering;
	switch (loadAction)
	{
	case TextureLoadAction::Load:
		return MTL::LoadActionLoad;
	case TextureLoadAction::Clear:
		return MTL::LoadActionClear;
	case TextureLoadAction::DoNothing:
	default:
		return MTL::LoadActionDontCare;
	}
}

MTL::StoreAction toMTLStoreAction(const kgx::rendering::TextureStoreAction& storeAction)
{
	using namespace kgx::rendering;
	switch (storeAction)
	{
	case TextureStoreAction::Store:
		return MTL::StoreActionStore;
	case TextureStoreAction::DoNothing:
	default:
		return MTL::StoreActionDontCare;
	}
}

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
MTLRenderContext::MTLRenderContext(core::ResourcePool<RHIRenderContext>& parentPool, MTLPlatform& platform)
	: RHIRenderContext(parentPool), mPlatform(platform)
{
}

bool MTLRenderContext::create()
{
	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice()->getNativeDevice();

	mCommandBuffer = NS::TransferPtr(mtlDevice->newCommandBuffer());
	if (!mCommandBuffer)
	{
		return false;
	}

	//TODO(KL): Pass in optional label to be used via constructor
	mCommandBuffer->setLabel(NS::String::string("MTLRenderContext_commandBuffer", NS::UTF8StringEncoding));

	return createArgumentTables();
}

	void MTLRenderContext::setConstantBuffers(
		const RHIBuffer* sceneConstantBuffer,
		const RHIBuffer* meshInstanceConstantBuffer,
		const RHIBuffer* objectIdsBuffer)
{
	mSceneConstantBuffer = static_cast<const MTLBuffer*>(sceneConstantBuffer);
	mMeshInstanceConstantBuffer = static_cast<const MTLBuffer*>(meshInstanceConstantBuffer);
	mObjectIdsBuffer = static_cast<const MTLBuffer*>(objectIdsBuffer);
}

void MTLRenderContext::activateRenderPass(const rendering::KGXRenderPassParameters& renderPassParameters)
{
	mCurrentRenderPassParameters = renderPassParameters;
	auto renderPassDescriptor = toMTLRenderPassDescriptor(renderPassParameters);

	mEncoder = mCommandBuffer->renderCommandEncoder(renderPassDescriptor.get(), MTL4::RenderEncoderOptionNone);

	const core::KGXViewport &viewport = renderPassParameters.viewport;
	MTL::Viewport mtlViewport =
	{
		.originX = static_cast<double>(viewport.topLeftX),
		.originY = static_cast<double>(viewport.topLeftY),
		.width = static_cast<double>(viewport.width),
		.height = static_cast<double>(viewport.height),
		.znear = static_cast<double>(viewport.minDepth),
		.zfar = static_cast<double>(viewport.maxDepth),
	};

	mEncoder->setViewport(mtlViewport);

	MTL::ScissorRect mtlScissorRect =
	{
		.x = 0, .y = 0, .width = viewport.width, .height = viewport.height
	};

	mEncoder->setScissorRect(mtlScissorRect);

	mEncoder->setFrontFacingWinding(MTL::WindingClockwise);
	mEncoder->setCullMode(MTL::CullModeBack);

	auto activePSO = static_cast<MTLGraphicsPipelineState*>(mCurrentRenderPassParameters.pso);
	mEncoder->setRenderPipelineState(activePSO->getPSO());

	std::array<IRDescriptorTableEntry, 2> bufferEntries{};

	IRDescriptorTableSetBuffer(&bufferEntries[0],
		mMeshInstanceConstantBuffer->getGPUAddress(),
		mMeshInstanceConstantBuffer->bufferSize());

	IRDescriptorTableSetBuffer(&bufferEntries[1],
		mSceneConstantBuffer->getGPUAddress(),
		mSceneConstantBuffer->bufferSize());

	//TODO(KL): Do this only once per frame so this has to be done by the KGXRenderScene somehow or perhaps the platform?
	setTopLevelBufferEntries(bufferEntries);

	MTL4::ArgumentTable* argumentTable = getArgumentTable();

	const RHIGraphicsPipelineStateDescriptor& psoDescriptor = activePSO->getDescriptor();

	if (psoDescriptor.vs)
	{
		mEncoder->setArgumentTable(argumentTable, MTL::RenderStageVertex);
	}

	if (psoDescriptor.ps)
	{
		mEncoder->setArgumentTable(argumentTable, MTL::RenderStageFragment);
	}
}

void MTLRenderContext::drawMeshRenderObject(const rendering::KGXMeshRenderObject* renderObject)
{
	MTLBuffer* vertexBuffer = rcCast(renderObject->getVertexBuffer());
	MTLBuffer* indexBuffer = rcCast(renderObject->getIndexBuffer());

	auto argumentTable = getArgumentTable();
	argumentTable->setAddress(vertexBuffer->getGPUAddress(), kIRVertexBufferBindPoint);
	argumentTable->setAddress(mObjectIdsBuffer->getGPUAddress(), kIRVertexBufferBindPoint + 1);

	mEncoder->drawIndexedPrimitives(
		MTL::PrimitiveTypeTriangle,
		renderObject->getNumIndices(),
		MTL::IndexTypeUInt16,
		indexBuffer->getGPUAddress(),
		indexBuffer->bufferSize(),
		1,
		0,
		renderObject->getObjectId()
	);
}

void MTLRenderContext::close()
{
	if (mEncoder)
	{
		mEncoder->endEncoding();
	}

	mCommandBuffer->endCommandBuffer();
}

void MTLRenderContext::execute(bool waitForCompletion)
{
	mPlatform.getCommandQueue().executeCommandBuffer(mCommandBuffer.get(), waitForCompletion);
}

void MTLRenderContext::reset()
{
	mSceneConstantBuffer = nullptr;
	mMeshInstanceConstantBuffer = nullptr;
	mEncoder = nullptr;
	mCommandAllocator = mPlatform.getCommandAllocator();
	mCommandBuffer->beginCommandBuffer(mCommandAllocator->getNativeAllocator());
}

void MTLRenderContext::releaseImpl()
{
	mCommandAllocator->reset();
	mCommandAllocator->release();
}

NS::SharedPtr<MTL4::RenderPassDescriptor> MTLRenderContext::toMTLRenderPassDescriptor(
	const rendering::KGXRenderPassParameters& renderPassParameters) const
{
	NS::SharedPtr<MTL4::RenderPassDescriptor> renderPassDescriptor = NS::TransferPtr(MTL4::RenderPassDescriptor::alloc()->init());

	for (int i = 0; i < renderPassParameters.outputTextures.size(); ++i)
	{
		const rendering::TextureBinding& textureBinding = renderPassParameters.outputTextures[i];

		auto* tex2D = static_cast<MTLTexture2D*>(mPlatform.getTexture2D(textureBinding.texture));
		if (!tex2D)
		{
			continue;
		}

		MTL::RenderPassColorAttachmentDescriptor* colorAttach = renderPassDescriptor->colorAttachments()->object(i);
		colorAttach->setTexture(tex2D->getTextureResource());
		colorAttach->setLoadAction(toMTLLoadAction(textureBinding.loadAction));
		colorAttach->setStoreAction(toMTLStoreAction(textureBinding.storeAction));
		colorAttach->setClearColor(toMTLClearColor(textureBinding.clearValue));
	}

	//TODO(KL): Depthstencil buffer

	/*if (mPassContext.depthStencilTexture.texture)
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
	}*/

	renderPassDescriptor->setDefaultRasterSampleCount(1);

	return renderPassDescriptor;
}

MTL4::ArgumentTable* MTLRenderContext::getArgumentTable() const
{
	const uint64_t argTableIndex = core::gRenderThread->getBufferedFrameIndex();
	return mArgumentTables[argTableIndex].get();
}

void MTLRenderContext::setTopLevelBufferEntries(const std::array<IRDescriptorTableEntry, 2>& bufferEntries) const
{
	const size_t entriesByteSize = bufferEntries.size() * sizeof(IRDescriptorTableEntry);
	const uint64_t bufferIndex = core::gRenderThread->getBufferedFrameIndex();

	if (mTopLevelBuffers[bufferIndex]->length() != entriesByteSize)
	{
		//TODO(KL): Temporary crash fix. Will be improved later.
		return;
	}

	memcpy(mTopLevelBuffers[bufferIndex]->contents(), bufferEntries.data(), entriesByteSize);
}

bool MTLRenderContext::createArgumentTables()
{
	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice()->getNativeDevice();

	NS::SharedPtr<MTL4::ArgumentTableDescriptor> argDesc = NS::TransferPtr(
		MTL4::ArgumentTableDescriptor::alloc()->init());
	argDesc->setMaxBufferBindCount(8);

	// Scene buffer and instance buffer
	constexpr int numShaderBuffers = 2;

	constexpr size_t topLevelBufferSize = numShaderBuffers * sizeof(IRDescriptorTableEntry);
	mTopLevelBuffers.reserve(core::RenderThread::maxNumBufferedFrames);
	mArgumentTables.reserve(core::RenderThread::maxNumBufferedFrames);

	//TODO(KL): Temporarily added to global residence set
	MTLCommandQueue& mtlCommandQueue = mPlatform.getCommandQueue();

	for (int i = 0; i < core::RenderThread::maxNumBufferedFrames; i++)
	{
		mTopLevelBuffers.push_back(NS::TransferPtr(mtlDevice->newBuffer(topLevelBufferSize, MTL::ResourceStorageModeShared)));

		auto& topLevelBuffer = mTopLevelBuffers.back();

		const std::string topLevelBufferName = std::format("TopLevelBuffer_{}", i);
		NS::String* topLevelBufferLabel = NS::String::string(topLevelBufferName.c_str(), NS::UTF8StringEncoding);
		topLevelBuffer->setLabel(topLevelBufferLabel);

		mtlCommandQueue.addGlobalResidency(topLevelBuffer.get());

		const std::string argumentBufferName = std::format("ArgumentBuffer_{}", i);
		NS::String* argumentBufferLabel = NS::String::string(topLevelBufferName.c_str(), NS::UTF8StringEncoding);
		argDesc->setLabel(argumentBufferLabel);

		NS::Error* error = nullptr;
		mArgumentTables.push_back(NS::TransferPtr(mtlDevice->newArgumentTable(argDesc.get(), &error)));
		MTLUtils::printIfNSError(error);

		mArgumentTables[i]->setAddress(mTopLevelBuffers[i]->gpuAddress(), kIRArgumentBufferBindPoint);
	}

	return mArgumentTables.size() == core::RenderThread::maxNumBufferedFrames;
}
}
