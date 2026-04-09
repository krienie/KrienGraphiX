
#include "MTLShader.h"

#include <Metal/Metal.hpp>
#include <metal_irconverter.h>
#include <metal_irconverter_runtime.h>
#include <Metal/MTLLibrary.hpp>
#include <Metal/MTL4ArgumentTable.hpp>
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

	return createArgumentTables(compiledShader);
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

MTL4::ArgumentTable* MTLShader::getArgumentTable() const
{
	const uint64_t argTableIndex = core::gRenderThread->getBufferedFrameIndex();
	return mArgumentTables[argTableIndex].get();
}

void MTLShader::setTopLevelBufferEntries(const std::vector<IRDescriptorTableEntry>& entries) const
{
	const size_t entriesByteSize = entries.size() * sizeof(IRDescriptorTableEntry);
	const uint64_t bufferIndex = core::gRenderThread->getBufferedFrameIndex();

	assert(mTopLevelBuffers[bufferIndex]->length() == entriesByteSize);
	memcpy(mTopLevelBuffers[bufferIndex]->contents(), entries.data(), entriesByteSize);
}

bool MTLShader::createArgumentTables(const CompiledShader& compiledShader)
{
	if (compiledShader.reflection.numResources <= 0)
	{
		mArgumentTables.resize(core::RenderThread::maxNumBufferedFrames);
		return true;
	}
	
	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice()->getNativeDevice();

	NS::SharedPtr<MTL4::ArgumentTableDescriptor> argDesc = NS::TransferPtr(
		MTL4::ArgumentTableDescriptor::alloc()->init());
	argDesc->setMaxBufferBindCount(7);

	const size_t topLevelBufferSize = compiledShader.reflection.numResources * sizeof(IRDescriptorTableEntry);
	mTopLevelBuffers.reserve(core::RenderThread::maxNumBufferedFrames);
	mArgumentTables.reserve(core::RenderThread::maxNumBufferedFrames);

	//TODO(KL): Temporarily added to global residence set
	MTLCommandQueue* mtlCommandQueue = rcCast(core::gRenderThread->getCommandQueuePtr());

	for (int i = 0; i < core::RenderThread::maxNumBufferedFrames; i++)
	{
		mTopLevelBuffers.push_back(NS::TransferPtr(mtlDevice->newBuffer(topLevelBufferSize, MTL::ResourceStorageModeShared)));
		mtlCommandQueue->addGlobalResidency(mTopLevelBuffers[i].get());

		NS::Error* error = nullptr;
		mArgumentTables.push_back(NS::TransferPtr(mtlDevice->newArgumentTable(argDesc.get(), &error)));
		MTLUtils::printIfNSError(error);

		assert(mArgumentTables[i].get() != nullptr);
		mArgumentTables[i]->setAddress(mTopLevelBuffers[i]->gpuAddress(), kIRArgumentBufferBindPoint);
	}

	return mArgumentTables.size() == core::RenderThread::maxNumBufferedFrames;
}
}
