
#include "MTLShader.h"

#include <Metal/Metal.hpp>
#include <metal_irconverter.h>
#include <Metal/MTLLibrary.hpp>
#include <Metal/MTL4LibraryFunctionDescriptor.hpp>

#include "MTLCommandQueue.h"
#include "MTLRenderHardwareInterface.h"
#include "MTLUtils.h"
#include "Private/Core/RenderThread.h"

namespace kgx::RHI
{
bool MTLShader::create(const CompiledShader& compiledShader, ShaderType type)
{
	// Kind of unused, but I will keep it here for now
	mShaderType = type;

	dispatch_data_t dispatchData = dispatch_data_create(compiledShader.byteCode.data(), compiledShader.byteCode.size(),
		nullptr, DISPATCH_DATA_DESTRUCTOR_DEFAULT);

	auto mtlDevice = getMTLRHI()->getMTLDevice()->getNativeDevice();

	NS::Error* error = nullptr;
	mLibrary = NS::TransferPtr(mtlDevice->newLibrary(dispatchData, &error));
	MTLUtils::printIfNSError(error);

	mLibraryFunctionDesc = NS::TransferPtr(
		MTL4::LibraryFunctionDescriptor::alloc()->init());
	mLibraryFunctionDesc->setLibrary(mLibrary.get());

	NS::String* mainEntry = NS::String::string(compiledShader.reflection.mainEntry.c_str(), NS::UTF8StringEncoding);
	mLibraryFunctionDesc->setName(mainEntry);

	return true;
}

void MTLShader::setVertexInputLayout(const std::vector<VertexInputElement>& vertexInputLayout)
{
	mVertexLayout.setFromInputElementVector(vertexInputLayout);
}

MTL::VertexDescriptor* MTLShader::getMTLVertexDescriptor() const
{
	return mVertexLayout.getNativeVertexDescriptor();
}

MTL4::LibraryFunctionDescriptor* MTLShader::getLibraryFunctionDescriptor() const
{
	return mLibraryFunctionDesc.get();
}
}
