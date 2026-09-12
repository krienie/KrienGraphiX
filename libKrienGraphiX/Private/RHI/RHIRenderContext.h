
#pragma once

#include "Private/Core/ResourcePool.h"

namespace kgx::rendering
{
struct KGXRenderPassParameters;
class KGXMeshRenderObject;
}

namespace kgx::RHI
{
class RHIBuffer;
class RHIPlatform;

class RHIRenderContext : public core::PooledType<RHIRenderContext>
{
public:
	RHIRenderContext(core::ResourcePool<RHIRenderContext>& parentPool);
	virtual ~RHIRenderContext() = default;

	virtual void setGlobalConstantBuffer(const RHIBuffer* constantBuffer) = 0;
	virtual void activateRenderPass(const rendering::KGXRenderPassParameters& renderPassParameters) = 0;
	virtual void drawMeshRenderObject(const rendering::KGXMeshRenderObject* renderObject) = 0;

	virtual void close() = 0;
	virtual void execute(bool waitForCompletion) = 0;
	virtual void reset() = 0;

protected:
	core::ResourcePool<RHIRenderContext>* mParentPool = nullptr;
};
}
