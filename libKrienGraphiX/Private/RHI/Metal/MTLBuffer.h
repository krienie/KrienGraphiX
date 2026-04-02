
#pragma once

#include <memory>

#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTLBuffer.hpp>

#include "Private/RHI/RHIBuffer.h"
#include "Private/RHI/RHIUtils.h"

namespace kgx::RHI
{
class MTLBuffer final : public RHIBuffer
{
public:
	MTLBuffer(const RHIBufferDescriptor& descriptor);
	~MTLBuffer() override = default;

	MTLBuffer(MTLBuffer&) = delete;
	MTLBuffer(MTLBuffer&&) noexcept = default;
	MTLBuffer& operator=(MTLBuffer&) = delete;
	MTLBuffer& operator=(MTLBuffer&&) noexcept = default;

	[[nodiscard]]
	MTL::GPUAddress getGPUAddress() const;
	
	[[nodiscard]]
	void* mappedDataPtr() const override;

private:
	void* mapImpl(MapType type) override;
	void unmapImpl() override;

	NS::SharedPtr<MTL::Buffer> mResource;
};

DEFINE_RESOURCE_CAST(MTLBuffer, RHIBuffer);
}
