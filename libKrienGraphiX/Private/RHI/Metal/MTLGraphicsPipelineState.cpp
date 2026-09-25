
#include "MTLGraphicsPipelineState.h"

#include <Metal/MTL4RenderPipeline.hpp>

#include "MTLPixelFormat.h"
#include "MTLRenderHardwareInterface.h"
#include "MTLShader.h"

namespace kgx::RHI
{
MTLGraphicsPipelineState::MTLGraphicsPipelineState(const RHIGraphicsPipelineStateDescriptor& desc)
	: RHIGraphicsPipelineState(desc)
{
}

bool MTLGraphicsPipelineState::create()
{
	NS::SharedPtr<MTL4::RenderPipelineDescriptor> psoDesc = NS::TransferPtr(
		MTL4::RenderPipelineDescriptor::alloc()->init());

	if (mGraphicsDescriptor.vs)
	{
		MTLShader* mtlVertexShader = rcCast(mGraphicsDescriptor.vs);

		psoDesc->setVertexDescriptor(mtlVertexShader->getMTLVertexDescriptor());
		psoDesc->setVertexFunctionDescriptor(mtlVertexShader->getLibraryFunctionDescriptor());
	}

	if (mGraphicsDescriptor.ps)
	{
		MTLShader* mtlPixelShader = rcCast(mGraphicsDescriptor.ps);
		psoDesc->setFragmentFunctionDescriptor(mtlPixelShader->getLibraryFunctionDescriptor());
	}

	for (unsigned int i = 0; i < mGraphicsDescriptor.numRenderTargets; ++i)
	{
		MTL4::RenderPipelineColorAttachmentDescriptor* renderTarget = psoDesc->colorAttachments()->object(i);
		renderTarget->setPixelFormat(toMTLPixelFormat(mGraphicsDescriptor.renderTargetFormats[i]));
		renderTarget->setBlendingState(MTL4::BlendStateDisabled);
	}

	psoDesc->setRasterizationEnabled(true);
	//TODO(KL): Implement MSAA support
	psoDesc->setRasterSampleCount(1);

	MTLGraphicsDevice* mtlDevice = getMTLRHI()->getMTLDevice();
	MTL4::Compiler* compiler = mtlDevice->getCompiler();

	NS::Error* pError = nullptr;
	mRenderPipelineState = NS::TransferPtr(compiler->newRenderPipelineState(psoDesc.get(), nullptr, &pError));

	return true;
}

MTL::RenderPipelineState* MTLGraphicsPipelineState::getPSO() const
{
	return mRenderPipelineState.get();
}
}
