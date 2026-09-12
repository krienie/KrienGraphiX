
#pragma once

#include <memory>

#include <Foundation/NSSharedPtr.hpp>
#include <Metal/Metal.hpp>
#include <Metal/MTL4CommandBuffer.hpp>
#include <Metal/MTL4RenderPass.hpp>

#include "MTLBuffer.h"
#include "MTLPlatform.h"
#include "Private/Rendering/KGXRenderPass.h"
#include "Private/RHI/RHIRenderContext.h"

namespace kgx::rendering
{
class KGXMeshRenderObject;
}

namespace kgx::RHI
{
class MTLCommandAllocator;

class MTLRenderContext : public RHIRenderContext
{
public:
	MTLRenderContext(core::ResourcePool<RHIRenderContext>& parentPool, MTLPlatform& platform);

	bool create();

	void setGlobalConstantBuffer(const RHIBuffer* constantBuffer) override;
	void activateRenderPass(const rendering::KGXRenderPassParameters& renderPassParameters) override;
	void drawMeshRenderObject(const rendering::KGXMeshRenderObject* renderObject) override;

	void close() override;
	void execute(bool waitForCompletion) override;
	void reset() override;

private:
	void releaseImpl() override;

	[[nodiscard]] NS::SharedPtr<MTL4::RenderPassDescriptor> toMTLRenderPassDescriptor(const rendering::KGXRenderPassParameters& renderPassParameters) const;

	MTLPlatform& mPlatform;
	NS::SharedPtr<MTL4::CommandBuffer> mCommandBuffer = nullptr;
	MTL4::RenderCommandEncoder* mEncoder = nullptr;
	MTLCommandAllocator* mCommandAllocator = nullptr;

	rendering::KGXRenderPassParameters mCurrentRenderPassParameters{};
	const MTLBuffer* mGlobalConstantBuffer = nullptr;
};
}
