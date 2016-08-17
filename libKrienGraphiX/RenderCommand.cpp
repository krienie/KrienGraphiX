
#include "RenderCommand.h"

namespace kgx
{
	namespace rendercommand
	{
		const BackendDispatchFunction BindMeshToIAStage::DISPATCH_FUNCTION            = &renderbackend::bindMeshToIAStage;
		const BackendDispatchFunction DrawIndexed::DISPATCH_FUNCTION                  = &renderbackend::drawIndexed;
		const BackendDispatchFunction CopyConstantBufferData::DISPATCH_FUNCTION       = &renderbackend::copyConstantBufferData;
		const BackendDispatchFunction UpdateConstantBufferVariable::DISPATCH_FUNCTION = &renderbackend::updateConstantBufferVariable;
		const BackendDispatchFunction SetVertexShaderSamplers::DISPATCH_FUNCTION      = &renderbackend::setVertexShaderSamplers;
		const BackendDispatchFunction SetPixelShaderSamplers::DISPATCH_FUNCTION       = &renderbackend::setPixelShaderSamplers;
		const BackendDispatchFunction SetVertexShaderResourceViews::DISPATCH_FUNCTION = &renderbackend::setVertexShaderResourceViews;
		const BackendDispatchFunction SetPixelShaderResourceViews::DISPATCH_FUNCTION  = &renderbackend::setPixelShaderResourceViews;
	}
}
