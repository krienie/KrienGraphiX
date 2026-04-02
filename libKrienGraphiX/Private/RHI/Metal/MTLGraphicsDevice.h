
#pragma once

#include <memory>
#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTL4Compiler.hpp>
#include <Metal/MTLDevice.hpp>

namespace kgx::RHI
{
class MTLGraphicsDevice
{
public:
	MTLGraphicsDevice();

	[[nodiscard]]
	MTL::Device* getNativeDevice() const;

	[[nodiscard]]
	MTL4::Compiler* getCompiler() const;

private:
	NS::SharedPtr<MTL::Device> mDevice;
	NS::SharedPtr<MTL4::Compiler> mCompiler;
};
}

