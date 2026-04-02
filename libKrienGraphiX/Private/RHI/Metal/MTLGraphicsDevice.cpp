
#include "MTLGraphicsDevice.h"

#include "Metal/MTL4Compiler.hpp"

namespace kgx::RHI
{
MTLGraphicsDevice::MTLGraphicsDevice()
{
	mDevice = NS::TransferPtr(MTL::CreateSystemDefaultDevice());

	NS::Error* error = nullptr;
	auto compilerDesc = NS::TransferPtr(MTL4::CompilerDescriptor::alloc()->init());
	mCompiler = NS::TransferPtr(mDevice->newCompiler(compilerDesc.get(), &error));

	//TODO(KL): Handle error
}

MTL::Device* MTLGraphicsDevice::getNativeDevice() const
{
	return mDevice.get();
}

MTL4::Compiler* MTLGraphicsDevice::getCompiler() const
{
	return mCompiler.get();
}
}
